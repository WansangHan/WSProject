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
	
	return std::shared_ptr<CPlayer>();
}

// ������ Ŭ���̾�Ʈ�� �ʱ� ������ġ�� ũ�⸦ ���ϰ� ����
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

	CPacketManager::getInstance().SendPacketToServer(_player->GetSocket(), SD_STARTING_POSITION_SCALE, sendData.SerializeAsString(), nullptr, true);
}

// �÷��̾� ���� ��
void CInGame::EnterPlayer(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size)
{
	WSSockServer::EnterServer RecvData;
	RecvData.ParseFromArray(_data, _size);

	// �Ѿ�� ID, name���� Player ������ ����
	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());
	player->SetName(RecvData._name());
	player->SetSocket(_sock);

	// map ������ �÷��̾� Insert
	m_players.insert(std::map<int, std::shared_ptr<CPlayer>>::value_type(player->GetID(), player));

	SetStartingPositionScale(player);
}

void CInGame::ExitPlayer(std::shared_ptr<CBaseSocket> _sock)
{
	// ���� �ڵ� ������ ���̵� ã�� �� erase
	int pID = FindIDToSOCKET(_sock);
	m_players.erase(pID);
}
