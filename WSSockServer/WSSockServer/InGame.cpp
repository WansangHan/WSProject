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

// InGame Ŭ���� �ʱ�ȭ
void CInGame::InitInGame()
{
	AllocateAIObject();
}

// ���� �ڵ鰪���� Ŭ���̾�Ʈ�� ���̵� ã�� �Լ�
int CInGame::FindIDToSOCKET(std::shared_ptr<CBaseSocket> _sock)
{
	// �÷��̾� ���� for
	for (auto Pr : m_players)
	{
		std::shared_ptr<CBaseSocket> socket = Pr.second->GetSocket();
		// ���� �ڵ鰪�� ���ٸ� ����
		if (socket->GetSOCKET() == _sock->GetSOCKET())
			return Pr.second->GetID();
	}
	// ��ġ�ϴ� ���� ���� ���� ���
	CLogManager::getInstance().WriteLogMessage("WARN", true, "FindIDToSOCKET return -1");
	return -1;
}

// �÷��̾� ���̵�� ��Ī�Ǵ� Player Ŭ���� ������ ã�� �Լ�
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

// AI ���̵�� ��Ī�Ǵ� AIObject Ŭ���� ������ ã�� �Լ�
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

// AI���� �ο��� ID ���� �������ִ� �Լ�
int CInGame::MakeAIObjectID()
{
	for (int i = 0;; i++) // 0���� ���Ѵ� ���ڱ��� ++
	{
		auto curVal = m_AIObjects.find(i);

		if (curVal == m_AIObjects.end())	// �ߺ��Ǵ� ID�� ���ٸ� ����
			return i;
	}
}

// ������ ��� �÷��̾�� ����
void CInGame::SendToAllPlayer(SendPacketType _type, std::string _str, sockaddr_in * _sockaddr, bool _isTCP)
{
	for (auto Pr : m_players)
	{
		std::shared_ptr<CBaseSocket> socket = Pr.second->GetSocket();
		CPacketManager::getInstance().SendPacketToServer(socket, _type, _str, _sockaddr, _isTCP);
	}
}

// AI ����
void CInGame::AllocateAIObject()
{
	std::shared_ptr<CAIObject> aiObject = std::make_shared<CAIObject>();

	int AIObjectID = MakeAIObjectID();
	aiObject->SetID(AIObjectID);

	// ������ AIObject�� map ������ ����
	m_AIObjects.insert(std::map<int, std::shared_ptr<CAIObject>>::value_type(aiObject->GetID(), aiObject));

	// AIObject�� ���� ��ġ, ũ�⸦ �޾ƿ��� ���� EPOLL ������ ��Ŷ�� ������.
	WSSockServer::ObjectInformation sendData;
	sendData.set__id(aiObject->GetID());
	CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_MAKE_AIOBJECT, sendData.SerializeAsString(), true);
}

// �÷��̾� ���� �� (Ŭ���̾�Ʈ�� ���� ��û)
void CInGame::EnterPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::PlayerInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	// �Ѿ�� ID, name���� Player ������ ����
	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());
	player->SetName(RecvData._name());
	player->SetSocket(_sock);

	// ���� ���� �÷��̾ ���� ������ �̹� ������ �ִ� �÷��̾�� ����
	SendToAllPlayer(SendPacketType::SD_ENTER_IOCP_SERVER, RecvData.SerializeAsString(), nullptr, true);

	// ���� ���� �÷��̾�� �� �÷��̾���� ������ ����
	for (auto Pr : m_players)
	{
		std::shared_ptr<CPlayer> player = Pr.second;

		WSSockServer::PlayerInformation SendData_ES;
		SendData_ES.set__id(player->GetID());
		SendData_ES.set__name(player->GetName());
		// �÷��̾� ���̵�, �̸� ����
		CPacketManager::getInstance().SendPacketToServer(_sock, SendPacketType::SD_ENTER_IOCP_SERVER, SendData_ES.SerializeAsString(), nullptr, true);

		WSSockServer::ObjectTransform SendData_SP;
		SendData_SP.set__id(player->GetID());
		SendData_SP.set__vectorx(player->GetTransform()->m_vectorX);
		SendData_SP.set__vectory(player->GetTransform()->m_vectorY);
		SendData_SP.set__scale(player->GetTransform()->m_scale);
		SendData_SP.set__speed(player->GetTransform()->m_speed);
		SendData_SP.set__dir((int)player->GetTransform()->m_dir);
		// �÷��̾� Transform ���� ����
		CPacketManager::getInstance().SendPacketToServer(_sock, SendPacketType::SD_PLAYER_POSITION_SCALE, SendData_SP.SerializeAsString(), nullptr, true);
	}

	// ���� ����ִ� AIObject ������ ������ �÷��̾�� ����
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

	// map ������ �÷��̾� Insert
	m_players.insert(std::map<int, std::shared_ptr<CPlayer>>::value_type(player->GetID(), player));

	// ������ Ŭ���̾�Ʈ�� ������ EPOLL ������ ����
	CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_ENTER_PEER, RecvData.SerializeAsString(), true);
}

// Ŭ���̾�Ʈ�� EPOLL ���� ���� ����
void CInGame::SuccessEnterEpoll(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	// EPOLL ������ ������ ������ �÷��̾�� �����ߴٴ� ��Ŷ�� ����
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

	// �������� ��� �÷��̾�� ���� ���� �÷��̾��� ��ġ ���� ����
	SendToAllPlayer(SendPacketType::SD_PLAYER_POSITION_SCALE, sendData.SerializeAsString(), nullptr, true);
}

void CInGame::ExitPlayer(std::shared_ptr<CBaseSocket> _sock)
{
	// ���� �ڵ� ������ ���̵� ã�� �� erase
	int pID = FindIDToSOCKET(_sock);
	if (pID == -1) return;	// ���Ͽ� �ش��ϴ� ���̵� ���� �� ���� (EPOLL ������ �� ����ó���� �ɸ�)

	m_players.erase(pID);

	// ���� �÷��̾��� ID�� �����ִ� ��� Ŭ���̾�Ʈ���� ����
	WSSockServer::PlayerInformation SendData;
	SendData.set__id(pID);
	SendToAllPlayer(SendPacketType::SD_EXIT_PLAYER, SendData.SerializeAsString(), nullptr, true);
	CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_EXIT_PEER, SendData.SerializeAsString(), true);
}

// �÷��̾� ������ UDP ��巹�� ����
void CInGame::ApplyPlayerUDP(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char * _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	player->SetAddr(_addr);

	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_IOCP_UDP, "", nullptr, true);
}

// Ŭ���̾�Ʈ�κ��� ���� �÷��̾� ��ġ, ����, ũ�������� ������ ���� �� ��� �÷��̾�, EPOLL ������ ����
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

// EPOLL �������� ���� ��ǥ�� AIObject�� �����Ŵ
void CInGame::ApplyAIObjectPositionScale(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CAIObject> aiObject = FindAIObjectToID(RecvData._id());
	std::shared_ptr<ObjectTransform> obejectTransform = std::make_shared<ObjectTransform>(RecvData._vectorx(), RecvData._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	aiObject->SetTransform(obejectTransform);
}
