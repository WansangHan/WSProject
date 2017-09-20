#include "stdafx.h"
#include "PlayManager.h"


CPlayManager::CPlayManager()
{
}


CPlayManager::~CPlayManager()
{
}

// �÷��̾� ���̵�� ��Ī�Ǵ� Player Ŭ���� ������ ã�� �Լ�
std::shared_ptr<CPlayer> CPlayManager::FindPlayerToID(int _pID)
{
	for (auto Pr : m_otherPlayer)
	{
		if (Pr->GetID() == _pID)
			return Pr;
	}
	CLogManager::getInstance().WriteLogMessage("WARN", true, "Return nullptr in FindPlayer()");
	return nullptr;
}

void CPlayManager::InitPlayerManager()
{
	m_ownPlayer = std::make_shared<CPlayer>();
}

void CPlayManager::PaintPlay(HWND _hwnd, HDC _hdc)
{
	m_ownPlayer->PaintPlayer(_hwnd, _hdc);
	for (auto Pr : m_otherPlayer)
	{
		Pr->PaintPlayer(_hwnd, _hdc);
	}
}

// ���� �÷��� �� �� ������ ������Ʈ �Ǵ� �κ�
void CPlayManager::UpdatePlay()
{
	m_ownPlayer->PlayerMove();
	for (auto Pr : m_otherPlayer)
	{
		Pr->PlayerMove();
	}
}

void CPlayManager::EnterGame()
{
	BattleCity::PlayerInformation sendData;

	sendData.set__id(m_ownPlayer->GetID());
	sendData.set__name(m_ownPlayer->GetName());

	// ���� Ŭ���̾�Ʈ�� ������ �޾� ������ ����
	CPacketManager::getInstance().SendPacketToServer(SendPacketType::SD_ENTER_SERVER, sendData.SerializeAsString(), true, true);
}

// �ڱ� �ڽ��� �ƴ� ��� �÷��̾ ���� ������ list�� �����Ѵ�
void CPlayManager::EnterPlayer(char * _data, int _size)
{
	BattleCity::PlayerInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());
	player->SetName(RecvData._name());
	
	// ��� �÷��̾ ����Ʈ�� �߰�
	m_otherPlayer.push_back(player);
}

// �ٸ� �÷��̾���� ������ �� ó��
void CPlayManager::ExitPlayer(char * _data, int _size)
{
	BattleCity::PlayerInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	// ���� �÷��̾ ã�� ����
	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	m_otherPlayer.remove(player);
}

// �����κ��� ���� ��ǥ, ũ��, ���� ������ �÷��̾ ����
void CPlayManager::SetPositionScale(char * _data, int _size)
{
	BattleCity::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	PlayerTransform playerTransform(RecvData._vectorx(), RecvData._vectory(), RecvData._scale(), RecvData._dir());
	// �ڱ� �ڽſ� ���� ��ġ �������
	if (m_ownPlayer->GetID() == RecvData._id())
	{
		m_ownPlayer->SetTransform(playerTransform);
	}
	else
	{
		// �ڱ� �ڽ��� �ƴ϶�� �÷��̾ ã�� Trasform ������ �����Ѵ�
		for (auto Pr : m_otherPlayer)
		{
			if (Pr->GetID() == RecvData._id())
			{
				Pr->SetTransform(playerTransform);
			}
		}
	}
}

// Ű���� �̺�Ʈ ��
void CPlayManager::CheckKey()
{
	m_ownPlayer->KeyCheck();
}
