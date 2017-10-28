#include "stdafx.h"
#include "InGame.h"

std::unique_ptr<CInGame> CInGame::m_inst;
std::once_flag CInGame::m_once;

CInGame::CInGame()
{
}


CInGame::~CInGame()
{
}

// InGame 클래스 초기화
void CInGame::InitInGame()
{
	AllocateAIObject();
}

// 소켓 핸들값으로 클라이언트의 아이디를 찾는 함수
int CInGame::FindIDToSOCKET(std::shared_ptr<CBaseSocket> _sock)
{
	// 플레이어 전부 for
	for (auto Pr : m_players)
	{
		std::shared_ptr<CBaseSocket> socket = Pr.second->GetSocket();
		// 소켓 핸들값이 같다면 리턴
		if (socket->GetSOCKET() == _sock->GetSOCKET())
			return Pr.second->GetID();
	}
	// 일치하는 소켓 값이 없는 경우
	CLogManager::getInstance().WriteLogMessage("WARN", true, "FindIDToSOCKET return -1");
	return -1;
}

// 플레이어 아이디와 매칭되는 Player 클래스 변수를 찾는 함수
std::shared_ptr<CPlayer> CInGame::FindPlayerToID(int _pID)
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

// AI 아이디와 매칭되는 AIObject 클래스 변수를 찾는 함수
std::shared_ptr<CAIObject> CInGame::FindAIObjectToID(int _pID)
{
	auto Ao = m_AIObjects.find(_pID);
	if (Ao != m_AIObjects.end())
		return Ao->second;
	else
	{
		CLogManager::getInstance().WriteLogMessage("WARN", true, "Return nullptr in FindAIObject()");
		return nullptr;
	}
}

// AI에게 부여할 ID 값을 리턴해주는 함수
int CInGame::MakeAIObjectID()
{
	for (int i = 0;; i++) // 0부터 무한대 숫자까지 ++
	{
		auto curVal = m_AIObjects.find(i);

		if (curVal == m_AIObjects.end())	// 중복되는 ID가 없다면 리턴
			return i;
	}
}

// 접속한 모든 플레이어에게 전송
void CInGame::SendToAllPlayer(SendPacketType _type, std::string _str, sockaddr_in * _sockaddr, bool _isTCP)
{
	for (auto Pr : m_players)
	{
		std::shared_ptr<CBaseSocket> socket = Pr.second->GetSocket();
		CPacketManager::getInstance().SendPacketToServer(socket, _type, _str, _sockaddr, _isTCP);
	}
}

// AI 생성
void CInGame::AllocateAIObject()
{
	std::shared_ptr<CAIObject> aiObject = std::make_shared<CAIObject>();

	int AIObjectID = MakeAIObjectID();
	aiObject->SetID(AIObjectID);

	// 생성한 AIObject를 map 변수에 저장
	m_AIObjects.insert(std::map<int, std::shared_ptr<CAIObject>>::value_type(aiObject->GetID(), aiObject));

	// AIObject의 시작 위치, 크기를 받아오기 위해 EPOLL 서버로 패킷을 보낸다.
	WSSockServer::ObjectInformation sendData;
	sendData.set__id(aiObject->GetID());
	CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_MAKE_AIOBJECT, sendData.SerializeAsString(), true);
}

// 플레이어 입장 시 (클라이언트의 접속 요청)
void CInGame::EnterPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::PlayerInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	// 넘어온 ID, name값을 Player 변수에 넣음
	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());
	player->SetName(RecvData._name());
	player->SetSocket(_sock);

	// 현재 들어온 플레이어에 대한 정보를 이미 접속해 있던 플레이어에게 전송
	SendToAllPlayer(SendPacketType::SD_ENTER_IOCP_SERVER, RecvData.SerializeAsString(), nullptr, true);

	// 현재 들어온 플레이어에게 각 플레이어들의 정보를 전송
	for (auto Pr : m_players)
	{
		std::shared_ptr<CPlayer> player = Pr.second;

		WSSockServer::PlayerInformation SendData_ES;
		SendData_ES.set__id(player->GetID());
		SendData_ES.set__name(player->GetName());
		// 플레이어 아이디, 이름 전송
		CPacketManager::getInstance().SendPacketToServer(_sock, SendPacketType::SD_ENTER_IOCP_SERVER, SendData_ES.SerializeAsString(), nullptr, true);

		WSSockServer::ObjectTransform SendData_SP;
		SendData_SP.set__id(player->GetID());
		SendData_SP.set__vectorx(player->GetTransform()->m_vectorX);
		SendData_SP.set__vectory(player->GetTransform()->m_vectorY);
		SendData_SP.set__scale(player->GetTransform()->m_scale);
		SendData_SP.set__speed(player->GetTransform()->m_speed);
		SendData_SP.set__dir((int)player->GetTransform()->m_dir);
		// 플레이어 Transform 정보 전송
		CPacketManager::getInstance().SendPacketToServer(_sock, SendPacketType::SD_PLAYER_POSITION_SCALE, SendData_SP.SerializeAsString(), nullptr, true);
	}

	// 현재 살아있는 AIObject 변수를 접속한 플레이어에게 전달
	for (auto Ao : m_AIObjects)
	{
		std::shared_ptr <CAIObject> aiObject = Ao.second;

		WSSockServer::ObjectTransform SendData;
		SendData.set__vectorx(aiObject->GetTransform()->m_vectorX);
		SendData.set__vectory(aiObject->GetTransform()->m_vectorY);
		SendData.set__scale(aiObject->GetTransform()->m_scale);
		SendData.set__speed(aiObject->GetTransform()->m_speed);
		SendData.set__dir((int)aiObject->GetTransform()->m_dir);
		CPacketManager::getInstance().SendPacketToServer(_sock, SendPacketType::SD_AIOBJECT_POSITION_SCALE, SendData.SerializeAsString(), nullptr, true);
	}

	// map 변수에 플레이어 Insert
	m_players.insert(std::map<int, std::shared_ptr<CPlayer>>::value_type(player->GetID(), player));

	// 접속한 클라이언트의 정보를 EPOLL 서버로 보냄
	CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_ENTER_PEER, RecvData.SerializeAsString(), true);
}

// 클라이언트의 EPOLL 서버 접속 성공
void CInGame::SuccessEnterEpoll(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	// EPOLL 서버에 접속을 성공한 플레이어에게 성공했다는 패킷을 보냄
	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_IOCP_CONNECT, "", nullptr, true);

	std::shared_ptr<ObjectTransform> playerTransform = std::make_shared<ObjectTransform>(RecvData._vectorx(), RecvData._vectory(), RecvData._scale(), RecvData._speed(), ObjectDirection::IDLE);
	player->SetTransform(playerTransform);

	WSSockServer::ObjectTransform sendData;
	sendData.set__id(player->GetID());
	sendData.set__vectorx(player->GetTransform()->m_vectorX);
	sendData.set__vectory(player->GetTransform()->m_vectorY);
	sendData.set__dir((int)player->GetTransform()->m_dir);
	sendData.set__scale(player->GetTransform()->m_scale);
	sendData.set__speed(player->GetTransform()->m_speed);

	// 접속중인 모든 플레이어에게 현재 들어온 플레이어의 위치 정보 전달
	SendToAllPlayer(SendPacketType::SD_PLAYER_POSITION_SCALE, sendData.SerializeAsString(), nullptr, true);
}

void CInGame::ExitPlayer(std::shared_ptr<CBaseSocket> _sock)
{
	// 소켓 핸들 값으로 아이디를 찾은 후 erase
	int pID = FindIDToSOCKET(_sock);
	if (pID == -1) return;	// 소켓에 해당하는 아이디가 없을 시 리턴 (EPOLL 서버도 이 예외처리에 걸림)

	m_players.erase(pID);

	// 나간 플레이어의 ID를 남아있는 모든 클라이언트에게 전송
	WSSockServer::PlayerInformation SendData;
	SendData.set__id(pID);
	SendToAllPlayer(SendPacketType::SD_EXIT_PLAYER, SendData.SerializeAsString(), nullptr, true);
	CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_EXIT_PEER, SendData.SerializeAsString(), true);
}

// 플레이어 변수에 UDP 어드레스 적용
void CInGame::ApplyPlayerUDP(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char * _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	player->SetAddr(_addr);

	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_IOCP_UDP, "", nullptr, true);
}

// 클라이언트로부터 받은 플레이어 위치, 방향, 크기정보를 서버에 적용 후 모든 플레이어, EPOLL 서버에 전송
void CInGame::ApplyPlayerPositionScale(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);
	
	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	std::shared_ptr<ObjectTransform> playerTransform = std::make_shared<ObjectTransform>(RecvData._vectorx(), RecvData._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	player->SetTransform(playerTransform);
	SendToAllPlayer(SendPacketType::SD_PLAYER_POSITION_SCALE, RecvData.SerializeAsString(), nullptr, true);

	CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_NOTIFY_PLAYER_TRANSFORM, RecvData.SerializeAsString(), true);
}

// EPOLL 서버에서 받은 좌표를 AIObject에 적용시킴
void CInGame::ApplyAIObjectPositionScale(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CAIObject> aiObject = FindAIObjectToID(RecvData._id());
	std::shared_ptr<ObjectTransform> obejectTransform = std::make_shared<ObjectTransform>(RecvData._vectorx(), RecvData._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	aiObject->SetTransform(obejectTransform);
}
