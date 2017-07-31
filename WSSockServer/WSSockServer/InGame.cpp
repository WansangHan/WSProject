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

// �÷��̾� ���� ��
void CInGame::EnterPlayer(std::shared_ptr<CBaseSocket> _sock, char* _data)
{
	WSSockServer::EnterServer RecvData;
	RecvData.ParseFromString(_data);

	// �Ѿ�� ID, name���� Player ������ ����
	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());
	player->SetName(RecvData._name());
	player->SetSocket(_sock);

	// map ������ �÷��̾� Insert
	m_players.insert(std::map<int, std::shared_ptr<CPlayer>>::value_type(player->GetID(), player));
}

void CInGame::ExitPlayer(std::shared_ptr<CBaseSocket> _sock)
{
	// ���� �ڵ� ������ ���̵� ã�� �� erase
	int pID = FindIDToSOCKET(_sock);
	m_players.erase(pID);
}
