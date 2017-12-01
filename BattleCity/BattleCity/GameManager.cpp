#include "stdafx.h"
#include "GameManager.h"

std::unique_ptr<CGameManager> CGameManager::m_inst;
std::once_flag CGameManager::m_once;

CGameManager::CGameManager()
{
}


CGameManager::~CGameManager()
{
	delete m_uiManager;
	delete m_playManager;
}

bool CGameManager::InitGameManager(HWND _hwnd)
{
	CCurlManager::getInstance().InitCurlManager();
	CLogManager::getInstance().InitLogManager();

	m_uiManager = new CUIManager;
	m_playManager = new CPlayManager;

	m_hwnd = _hwnd;
	m_gameState = INTRO;
	isConnected = false;

	m_uiManager->InitUIManager();
	m_playManager->InitPlayerManager();

	SetTimer(m_hwnd, 1, 1, NULL);

	return false;
}

void CGameManager::ExitGameManager()
{
	KillTimer(m_hwnd, 1);
	// ������ ���� �Ǿ��־��ٸ�
	if (isConnected)
	{
		CNetworkManager::getInstance().ExitNetworkManager();
		CPacketManager::getInstance().ExitPacketManager();
	}
}

void CGameManager::Timer(HWND _hwnd)
{
	switch (m_gameState)
	{
	case PLAY:
		// ������ �������� �� ȭ���� ��� ������
		InvalidateRect(_hwnd, NULL, TRUE);
		m_playManager->UpdatePlay();
		break;
	}
	CPacketManager::getInstance().APPLY_PACKET();
}

bool CGameManager::PaintAll()
{
	m_hdc = BeginPaint(m_hwnd, &ps);
	switch (m_gameState)
	{
	case INTRO:
		m_uiManager->PaintUI(m_hwnd, m_hdc);
		break;
	case PLAY:
		m_playManager->PaintPlay(m_hwnd, m_hdc);
		break;
	}
	EndPaint(m_hwnd, &ps);
	return true;
}

UINT CGameManager::Nchittest(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	// ������ â �̵��� ���� �ڵ�
	UINT nHit = DefWindowProc(_hwnd, _message, _wParam, _lParam);
	if (nHit == HTCLIENT)
		nHit = HTCAPTION;
	return nHit;
}

void CGameManager::CommandHandling(HWND _hwnd, WPARAM _wParam)
{
	m_uiManager->CommandHandling(_hwnd, _wParam);
}

void CGameManager::LButtonDown(LPARAM lParam)
{
	switch (m_gameState)
	{
	case INTRO:
		m_uiManager->MouceLButtonDown(m_hwnd, lParam);
		break;
	}
}

// Ű���� �̺�Ʈ ��
void CGameManager::KeyChange()
{
	switch (m_gameState)
	{
	case PLAY:
		m_playManager->CheckKey();
		break;
	}
}

void CGameManager::EnterGame(int _id, std::string _name)
{
	m_gameState = LODDING;
	// ������ ����
	CNetworkManager::getInstance().InitNetworkManager();
	CPacketManager::getInstance().InitPacketManager();
	isConnected = true;
	m_playManager->EnterGame(_id, _name);
}

// ����ȭ ������ ���� ���� ��
void CGameManager::SyncConnectSuccess(char * _data, int _size)
{
	BattleCity::ObjectInformation SendData;
	SendData.set__id(m_playManager->GetOwnPlayer()->GetID());
	CPacketManager::getInstance().SendPacketToServer(SendPacketType::SD_ENTER_CALC_SERVER, SendData.SerializeAsString(), true, true);
}

// EPOLL �������� ������ �Ϸ� �Ǹ�, �÷��̷� ��ȯ
void CGameManager::CompleteConnect(char * _data, int _size)
{
	m_gameState = PLAY;

	CNetworkManager::getInstance().NotifyUDPSocket();
}

// �÷��̾ ������ ��
void CGameManager::EnterPlayer(char * _data, int _size)
{
	m_playManager->EnterPlayer(_data, _size);
}

// �ٸ� �÷��̾���� ������ �� ó��
void CGameManager::ExitPlayer(char * _data, int _size)
{
	m_playManager->ExitPlayer(_data, _size);
}

// Player Transform ��Ŷ�� ���� ó��
void CGameManager::SetPlayerPositionScale(char* _data, int _size)
{
	m_playManager->SetPlayerPositionScale(_data, _size);
}

// AI Transform ��Ŷ�� ���� ó��
void CGameManager::SetAIObjectPositionScale(char * _data, int _size)
{
	m_playManager->SetAIObjectPositionScale(_data, _size);
}

// �ڱ� �ڽ� ���, ���� ����ó��
void CGameManager::DeathNotify(char * _data, int _size)
{
	SendMessage(m_hwnd, WM_DESTROY, 0, 0);
}

// �÷��̾��� ũ�� ����
void CGameManager::IncreaseScale(char * _data, int _size)
{
	m_playManager->IncreaseScale(_data, _size);
}
