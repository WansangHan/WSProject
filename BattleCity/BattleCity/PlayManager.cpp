#include "stdafx.h"
#include "PlayManager.h"
#include "BattleCity.pb.h"
#include "LogManager.h"
#include "PacketManager.h"


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
}

void CPlayManager::PaintPlay(HWND _hwnd, HDC _hdc)
{
	m_ownPlayer->PaintObject(_hwnd, _hdc);
	for (auto Pr : m_otherPlayer)
	{
		Pr->PaintObject(_hwnd, _hdc);
	}
	for (auto Ao : m_AIObjects)
	{
		Ao->PaintObject(_hwnd, _hdc);
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

void CPlayManager::EnterGame(int _id, std::string _name)
{
	m_ownPlayer = std::make_shared<CPlayer>();
	m_ownPlayer->SetID(_id);
	m_ownPlayer->SetName(_name);

	BattleCity::PlayerInformation sendData;
	sendData.set__id(_id);
	sendData.set__name(_name);

	// ���� Ŭ���̾�Ʈ�� ������ �޾� ������ ����
	CPacketManager::getInstance().SendPacketToServer(SendPacketType::SD_ENTER_SYNC_SERVER, sendData.SerializeAsString(), true, false);
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
void CPlayManager::SetPlayerPositionScale(char * _data, int _size)
{
	BattleCity::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	ObjectTransform playerTransform(RecvData._position()._vectorx(), RecvData._position()._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	// �ڱ� �ڽſ� ���� ��ġ �������
	if (m_ownPlayer->GetID() == RecvData._position()._id())
	{
		m_ownPlayer->SetTransform(playerTransform);
		m_ownPlayer->SetLastGetTickCount(GetTickCount());
	}
	else
	{
		// �ڱ� �ڽ��� �ƴ϶�� �÷��̾ ã�� Trasform ������ �����Ѵ�
		for (auto Pr : m_otherPlayer)
		{
			if (Pr->GetID() == RecvData._position()._id())
			{
				Pr->SetTransform(playerTransform);
				Pr->SetLastGetTickCount(GetTickCount());
			}
		}
	}
}

// �����κ��� ���� Transform ������ ���� AIObject�� ����� List�� ����
void CPlayManager::SetAIObjectPositionScale(char * _data, int _size)
{
	BattleCity::ObjectTransform RecvData;

	RecvData.ParseFromArray(_data, _size);
	ObjectTransform aiObjectTransform(RecvData._position()._vectorx(), RecvData._position()._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	
	std::shared_ptr<CAIObject> aiObject = std::make_shared<CAIObject>();
	aiObject->SetID(RecvData._position()._id());
	aiObject->SetTransform(aiObjectTransform);
	aiObject->SetCurTransform(aiObjectTransform);

	m_AIObjects.push_back(aiObject);
}

// �÷��̾��� ũ�� ����
void CPlayManager::IncreaseScale(char * _data, int _size)
{
	BattleCity::IncreaseScale RecvData;
	RecvData.ParseFromArray(_data, _size);
	// �ڱ� �ڽſ� ���� ��ġ �������
	if (m_ownPlayer->GetID() == RecvData._id())
	{
		std::shared_ptr<ObjectTransform> temp = m_ownPlayer->GetTransform();
		temp->m_scale = RecvData._increase();
	}
	else
	{
		// �ڱ� �ڽ��� �ƴ϶�� �÷��̾ ã�� Trasform ������ �����Ѵ�
		for (auto Pr : m_otherPlayer)
		{
			if (Pr->GetID() == RecvData._id())
			{
				std::shared_ptr<ObjectTransform> temp = Pr->GetTransform();
				temp->m_scale = RecvData._increase();
			}
		}
	}
}

// Ű���� �̺�Ʈ ��
void CPlayManager::CheckKey()
{
	m_ownPlayer->KeyCheck();
}
