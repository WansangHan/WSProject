#include "stdafx.h"
#include "PlayManager.h"


CPlayManager::CPlayManager()
{
}


CPlayManager::~CPlayManager()
{
	delete m_ownPlayer;
}

void CPlayManager::InitPlayerManager()
{
	m_ownPlayer = new CPlayer;
}

void CPlayManager::PaintPlay(HWND _hwnd, HDC _hdc)
{
	m_ownPlayer->PaintPlayer(_hwnd, _hdc);
}

// ���� �÷��� �� �� ������ ������Ʈ �Ǵ� �κ�
void CPlayManager::UpdatePlay()
{
	m_ownPlayer->PlayerMove();
}

void CPlayManager::EnterGame()
{
	BattleCity::EnterServer sendData;

	sendData.set__id(m_ownPlayer->GetID());
	sendData.set__name(m_ownPlayer->GetName());

	// ���� Ŭ���̾�Ʈ�� ������ �޾� ������ ����
	CPacketManager::getInstance().SendPacketToServer(SendPacketType::SD_ENTER_SERVER, sendData.SerializeAsString(), true, true);
}

// �����κ��� ���� ��ǥ, ũ��, ���� ������ �÷��̾ ����
void CPlayManager::SetPositionScale(char * _data, int _size)
{
	BattleCity::SetPositionScale RecvData;
	RecvData.ParseFromArray(_data, _size);

	if (m_ownPlayer->GetID() == RecvData._id())
	{
		m_ownPlayer->SetXY(RecvData._vectorx(), RecvData._vectory());
		m_ownPlayer->SetScale(RecvData._scale());
		m_ownPlayer->SetDir(RecvData._dir());
	}
}

// Ű���� �̺�Ʈ ��
void CPlayManager::CheckKey()
{
	m_ownPlayer->KeyCheck();
}
