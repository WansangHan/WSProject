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

// ������ Ŭ���̾�Ʈ�� �ʱ� ������ġ�� ũ�⸦ ���ϰ� ����
PlayerTransform CInGame::SetStartingTransform()
{
	float vectorX = 100.f;
	float vectorY = 100.f;
	float scale = 10.f;

	PlayerTransform playerTransform(vectorX, vectorY, scale, PlayerDirection::IDLE);

	return playerTransform;
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

// �÷��̾� ���� ��
void CInGame::EnterPlayer(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size)
{
	WSSockServer::PlayerInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	// �Ѿ�� ID, name���� Player ������ ����
	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());
	player->SetName(RecvData._name());
	player->SetSocket(_sock);

	// ���� ���� �÷��̾ ���� ������ �̹� ������ �ִ� �÷��̾�� ����
	SendToAllPlayer(SD_ENTER_SERVER, RecvData.SerializeAsString(), nullptr, true);

	// ���� ���� �÷��̾�� �� �÷��̾���� ������ ����
	for (auto Pr : m_players)
	{
		std::shared_ptr<CPlayer> player = Pr.second;

		WSSockServer::PlayerInformation SendData_ES;
		SendData_ES.set__id(player->GetID());
		SendData_ES.set__name(player->GetName());
		// �÷��̾� ���̵�, �̸� ����
		CPacketManager::getInstance().SendPacketToServer(_sock, SD_ENTER_SERVER, SendData_ES.SerializeAsString(), nullptr, true);

		WSSockServer::PlayerTransform SendData_SP;
		SendData_SP.set__id(player->GetID());
		SendData_SP.set__vectorx(player->GetTransform().m_vectorX);
		SendData_SP.set__vectory(player->GetTransform().m_vectorY);
		SendData_SP.set__scale(player->GetTransform().m_scale);
		SendData_SP.set__dir(player->GetTransform().m_dir);
		// �÷��̾� Transform ���� ����
		CPacketManager::getInstance().SendPacketToServer(_sock, SD_POSITION_SCALE, SendData_SP.SerializeAsString(), nullptr, true);
	}

	// map ������ �÷��̾� Insert
	m_players.insert(std::map<int, std::shared_ptr<CPlayer>>::value_type(player->GetID(), player));

	PlayerTransform playerTransform = SetStartingTransform();
	player->SetTransform(playerTransform);

	WSSockServer::PlayerTransform sendData;
	sendData.set__id(player->GetID());
	sendData.set__vectorx(player->GetTransform().m_vectorX);
	sendData.set__vectory(player->GetTransform().m_vectorY);
	sendData.set__dir(player->GetTransform().m_dir);
	sendData.set__scale(player->GetTransform().m_scale);

	// �������� ��� �÷��̾�� ���� ���� �÷��̾��� ��ġ ���� ����
	SendToAllPlayer(SD_POSITION_SCALE, sendData.SerializeAsString(), nullptr, true);
}

void CInGame::ExitPlayer(std::shared_ptr<CBaseSocket> _sock)
{
	// ���� �ڵ� ������ ���̵� ã�� �� erase
	int pID = FindIDToSOCKET(_sock);
	m_players.erase(pID);

	// ���� �÷��̾��� ID�� �����ִ� ��� Ŭ���̾�Ʈ���� ����
	WSSockServer::PlayerInformation SendData;
	SendData.set__id(pID);
	SendToAllPlayer(SD_EXIT_PLAYER, SendData.SerializeAsString(), nullptr, true);
}

// Ŭ���̾�Ʈ�κ��� ���� �÷��̾� ��ġ, ����, ũ�������� ������ ���� �� ��� �÷��̾�� ����
void CInGame::ApplyPlayerPositionScale(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size)
{
	WSSockServer::PlayerTransform RecvData;
	RecvData.ParseFromArray(_data, _size);
	
	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	PlayerTransform playerTransform(RecvData._vectorx(), RecvData._vectory(), RecvData._scale(), RecvData._dir());
	player->SetTransform(playerTransform);
	SendToAllPlayer(SD_POSITION_SCALE, RecvData.SerializeAsString(), nullptr, true);
}
