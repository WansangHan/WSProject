#include "stdafx.h"
#include "InGame.h"
#include "LogManager.h"
#include "WSSockServer.pb.h"

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

	CLogManager::getInstance().WriteLogMessage("INFO", true, "Allocate AIObject, ID : %d", AIObjectID);

	// ������ AIObject�� map ������ ����
	m_AIObjects.insert(std::map<int, std::shared_ptr<CAIObject>>::value_type(aiObject->GetID(), aiObject));

	// AIObject�� ���� ��ġ, ũ�⸦ �޾ƿ��� ���� ���� ������ ��Ŷ�� ������.
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
	SendToAllPlayer(SendPacketType::SD_ENTER_SYNC_SERVER, RecvData.SerializeAsString(), nullptr, true);

	// ���� ���� �÷��̾�� �� �÷��̾���� ������ ����
	for (auto Pr : m_players)
	{
		WSSockServer::PlayerInformation SendData_ES;
		SendData_ES.set__id(Pr.second->GetID());
		SendData_ES.set__name(Pr.second->GetName());
		// �÷��̾� ���̵�, �̸� ����
		CPacketManager::getInstance().SendPacketToServer(_sock, SendPacketType::SD_ENTER_SYNC_SERVER, SendData_ES.SerializeAsString(), nullptr, true);

		WSSockServer::ObjectPosition* pos = new WSSockServer::ObjectPosition;
		pos->set__id(Pr.second->GetID());
		pos->set__vectorx(Pr.second->GetTransform()->m_vectorX);
		pos->set__vectory(Pr.second->GetTransform()->m_vectorY);
		WSSockServer::ObjectTransform SendData_SP;
		SendData_SP.set_allocated__position(pos);
		SendData_SP.set__scale(Pr.second->GetTransform()->m_scale);
		SendData_SP.set__speed(Pr.second->GetTransform()->m_speed);
		SendData_SP.set__dir((int)Pr.second->GetTransform()->m_dir);
		// �÷��̾� Transform ���� ����
		CPacketManager::getInstance().SendPacketToServer(_sock, SendPacketType::SD_PLAYER_POSITION_SCALE, SendData_SP.SerializeAsString(), nullptr, true);
	}

	// ���� ����ִ� AIObject ������ ������ �÷��̾�� ����
	for (auto Ao : m_AIObjects)
	{
		std::shared_ptr <CAIObject> aiObject = Ao.second;

		WSSockServer::ObjectPosition* pos = new WSSockServer::ObjectPosition;
		pos->set__id(aiObject->GetID());
		pos->set__vectorx(aiObject->GetTransform()->m_vectorX);
		pos->set__vectory(aiObject->GetTransform()->m_vectorY);
		WSSockServer::ObjectTransform SendData;
		SendData.set_allocated__position(pos);
		SendData.set__scale(aiObject->GetTransform()->m_scale);
		SendData.set__speed(aiObject->GetTransform()->m_speed);
		SendData.set__dir((int)aiObject->GetTransform()->m_dir);
		CPacketManager::getInstance().SendPacketToServer(_sock, SendPacketType::SD_AIOBJECT_POSITION_SCALE, SendData.SerializeAsString(), nullptr, true);
	}

	// map ������ �÷��̾� Insert
	m_players.insert(std::map<int, std::shared_ptr<CPlayer>>::value_type(player->GetID(), player));

	// ������ Ŭ���̾�Ʈ�� ������ ���� ������ ����
	CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_ENTER_PEER, RecvData.SerializeAsString(), true);
}

// Ŭ���̾�Ʈ�� ���� ���� ���� ����
void CInGame::SuccessEnterCalc(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	// ���� ������ ������ ������ �÷��̾�� �����ߴٴ� ��Ŷ�� ����
	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._position()._id());
	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_CALC_CONNECT, "", nullptr, true);

	ObjectTransform playerTransform(RecvData._position()._vectorx(), RecvData._position()._vectory(), RecvData._scale(), RecvData._speed(), ObjectDirection::IDLE);
	player->SetTransform(playerTransform);

	WSSockServer::ObjectPosition* pos = new WSSockServer::ObjectPosition;
	pos->set__id(player->GetID());
	pos->set__vectorx(player->GetTransform()->m_vectorX);
	pos->set__vectory(player->GetTransform()->m_vectorY);
	WSSockServer::ObjectTransform sendData;
	sendData.set_allocated__position(pos);
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
	// ���Ͽ� �ش��ϴ� ���̵� ���� �� ����
	if (pID == -1)
	{
		CLogManager::getInstance().WriteLogMessage("WARN", true, "Unkown Socket Exit");
		return;
	}

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

	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_SYNC_UDP, "", nullptr, true);
}

// Ŭ���̾�Ʈ�κ��� ���� �÷��̾� ��ġ, ����, ũ�������� ������ ���� �� ��� �÷��̾�, ���� ������ ����
void CInGame::ApplyPlayerPositionScale(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);
	
	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._position()._id());
	ObjectTransform playerTransform(RecvData._position()._vectorx(), RecvData._position()._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	player->SetTransform(playerTransform);
	SendToAllPlayer(SendPacketType::SD_PLAYER_POSITION_SCALE, RecvData.SerializeAsString(), nullptr, true);

	CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_NOTIFY_PLAYER_TRANSFORM, RecvData.SerializeAsString(), true);
}

// ���� �������� ���� ��ǥ�� AIObject�� �����Ŵ
void CInGame::ApplyAIObjectPositionScale(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CAIObject> aiObject = FindAIObjectToID(RecvData._position()._id());
	std::shared_ptr<ObjectTransform> obejectTransform = std::make_shared<ObjectTransform>(RecvData._position()._vectorx(), RecvData._position()._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	aiObject->SetTransform(obejectTransform);
}

// 
void CInGame::CollisionNotify(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char * _data, int _size)
{
	WSSockServer::CollisionNotify RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> smallPlayer = FindPlayerToID(RecvData.smallplayerid());
	std::shared_ptr<CPlayer> bigerPlayer = FindPlayerToID(RecvData.bigerplayerid());

	// �÷��̾� ����
	{
		// �̹� ó���� �÷��̾��� ���
		if (smallPlayer == nullptr)
		{
			CLogManager::getInstance().WriteLogMessage("WARN", true, "Already Erase");
			return;
		}

		// ���� �÷��̾�� �׾��ٴ� �� �˸�
		CPacketManager::getInstance().SendPacketToServer(smallPlayer->GetSocket(), SendPacketType::SD_DEATH_NOTIFY, "", nullptr, true);

		m_players.erase(RecvData.smallplayerid());

		WSSockServer::PlayerInformation SendData;
		SendData.set__id(RecvData.smallplayerid());
		// ���� �÷��̾��� ID�� �����ִ� ��� Ŭ���̾�Ʈ, ������ ����
		SendToAllPlayer(SendPacketType::SD_EXIT_PLAYER, SendData.SerializeAsString(), nullptr, true);
		CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_EXIT_PEER, SendData.SerializeAsString(), true);
	}

	// �÷��̾� ũ�� ����
	{
		std::shared_ptr<ObjectTransform> temp = bigerPlayer->GetTransform();
		temp->m_scale += smallPlayer->GetTransform()->m_scale;
		bigerPlayer->SetTransform(*temp.get());

		WSSockServer::IncreaseScale SendData;
		SendData.set__id(bigerPlayer->GetID());
		SendData.set__increase(temp->m_scale);
		// ũ�Ⱑ ������ �÷��̾��� ���ο� ������ ��� Ŭ���̾�Ʈ, ������ ����
		SendToAllPlayer(SendPacketType::SD_INCREASE_SCALE_TO_PLAYER, SendData.SerializeAsString(), nullptr, true);
		CCalculateServer::getInstance().SendToCalculateServer(SendPacketType::SD_INCREASE_SCALE_TO_CALC, SendData.SerializeAsString(), true);
	}
}
