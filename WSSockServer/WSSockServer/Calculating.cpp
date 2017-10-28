#include "stdafx.h"
#include "Calculating.h"

std::unique_ptr<CCalculating> CCalculating::m_inst;
std::once_flag CCalculating::m_once;

CCalculating::CCalculating()
{
}


CCalculating::~CCalculating()
{
	isContinue = false;
	m_calculate_Thread->join();
}

// 플레이어 아이디와 매칭되는 Player 클래스 변수를 찾는 함수
std::shared_ptr<CPlayer> CCalculating::FindPlayerToID(int _pID)
{
	auto Pr = m_players.find(_pID);
	if (Pr != m_players.end())
		return Pr->second;
	else
	{
		CLogManager::getInstance().WriteLogMessage("WARN", true, "Return nullptr in FindPlayer()");
		return nullptr;
	}
}

// 모든 연산처리를 호출할 함수(쓰레드로 호출)
void CCalculating::CalculateAll()
{
	while (isContinue)
	{
		// 플레이어의 현재 좌표를 계산
		for (auto Pr : m_players)
		{
			if(Pr.second != nullptr)
				Pr.second->CalculateCurrentPosition();
		}
	}
}

void CCalculating::InitCalculating()
{
	srand((unsigned int)time(NULL));

	isContinue = true;

	m_calculate_Thread = std::unique_ptr<std::thread>(new std::thread([&]() { this->CalculateAll(); }));
}

// 오브젝트들의 시작 위치를 정해서 IOCP로 보내준다
void CCalculating::SetStartingPosition(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	float vectorX = rand() % 601;
	float vectorY = rand() % 601;
	float scale = rand() % 50;
	float speed = 20;

	WSSockServer::ObjectTransform SendData;
	SendData.set__id(RecvData._id());
	SendData.set__vectorx(vectorX);
	SendData.set__vectory(vectorY);
	SendData.set__scale(scale);
	SendData.set__speed(speed);
	SendData.set__dir((int)ObjectDirection::IDLE);
	CSyncServer::getInstance().SendToSyncServer(SendPacketType::SD_AI_STARTING, SendData.SerializeAsString(), true);
}

// 플레이어 접속 시 EPOLL 서버에도 클라이언트 정보를 저장 및 시작 좌표 리턴
void CCalculating::EnterPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::PlayerInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	float vectorX = rand() % 601;
	float vectorY = rand() % 601;
	float scale = rand() % 50;
	float speed = 20;

	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());

	std::shared_ptr<ObjectTransform> playerTransform = std::make_shared<ObjectTransform>(vectorX, vectorY, scale, speed, ObjectDirection::IDLE);
	player->SetTransform(playerTransform);
	player->SetCurTransform(playerTransform);
	player->SetLastGetTickCount(GetTickCount());

	m_players.insert(std::map<int, std::shared_ptr<CPlayer>>::value_type(player->GetID(), player));

	WSSockServer::ObjectTransform SendData;
	SendData.set__id(RecvData._id());
	SendData.set__vectorx(vectorX);
	SendData.set__vectory(vectorY);
	SendData.set__scale(scale);
	SendData.set__speed(speed);
	CSyncServer::getInstance().SendToSyncServer(SendPacketType::SD_ENTER_PLAYER_EPOLL, SendData.SerializeAsString(), true);
}

// 플레이어 변수에 소켓 적용
void CCalculating::ApplyPlayerSocket(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());

	player->SetSocket(_sock);

	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_EPOLL_CONNECT, "", nullptr, true);
}

// 플레이어 변수에 UDP 어드레스 적용
void CCalculating::ApplyPlayerUDP(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char * _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	player->SetAddr(_addr);

	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_EPOLL_UDP, "", nullptr, true);
}

// 플레이어가 나갔을 때, 해당 아이디에 맞는 플레이어 삭제
void CCalculating::ExitPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	m_players.erase(RecvData._id());
}

// 클라이언트의 마지막 이동 이벤트 정보를 저장
void CCalculating::ApplyPlayerTrasform(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char * _data, int _size)
{
	WSSockServer::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	player->SetLastGetTickCount(GetTickCount());
	std::shared_ptr<ObjectTransform> playerTransform = std::make_shared<ObjectTransform>(RecvData._vectorx(), RecvData._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	player->SetTransform(playerTransform);
}