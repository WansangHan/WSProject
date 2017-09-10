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
	
	return std::shared_ptr<CPlayer>();
}

// 접속한 클라이언트의 초기 시작위치와 크기를 정하고 전송
void CInGame::SetStartingPositionScale(std::shared_ptr<CPlayer> _player)
{
	float vectorX = 100.f;
	float vectorY = 100.f;
	float scale = 10.f;

	_player->SetXY(vectorX, vectorY);
	_player->SetScale(scale);
	_player->SetDir(PlayerDirection::IDLE);

	WSSockServer::SetPositionScale sendData;
	sendData.set__id(_player->GetID());
	sendData.set__vectorx(_player->GetX());
	sendData.set__vectory(_player->GetY());
	sendData.set__dir(_player->GetDir());
	sendData.set__scale(_player->GetScale());

	CPacketManager::getInstance().SendPacketToServer(_player->GetSocket(), SD_POSITION_SCALE, sendData.SerializeAsString(), nullptr, true);
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

// 플레이어 입장 시
void CInGame::EnterPlayer(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size)
{
	WSSockServer::EnterServer RecvData;
	RecvData.ParseFromArray(_data, _size);

	// 넘어온 ID, name값을 Player 변수에 넣음
	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());
	player->SetName(RecvData._name());
	player->SetSocket(_sock);

	// map 변수에 플레이어 Insert
	m_players.insert(std::map<int, std::shared_ptr<CPlayer>>::value_type(player->GetID(), player));

	SetStartingPositionScale(player);
}

void CInGame::ExitPlayer(std::shared_ptr<CBaseSocket> _sock)
{
	// 소켓 핸들 값으로 아이디를 찾은 후 erase
	int pID = FindIDToSOCKET(_sock);
	m_players.erase(pID);
}

// 클라이언트로부터 받은 플레이어 위치, 방향, 크기정보를 서버에 적용 후 모든 플레이어에게 전송
void CInGame::ApplyPlayerPositionScale(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size)
{
	WSSockServer::SetPositionScale RecvData;
	RecvData.ParseFromArray(_data, _size);
	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	player->SetXY(RecvData._vectorx(), RecvData._vectory());
	player->SetScale(RecvData._scale());
	player->SetDir(RecvData._dir());

	SendToAllPlayer(SD_POSITION_SCALE, RecvData.SerializeAsString(), nullptr, true);
}
