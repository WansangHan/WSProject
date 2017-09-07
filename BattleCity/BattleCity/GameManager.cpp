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
	CNetworkManager::getInstance().InitNetworkManager();
	CPacketManager::getInstance().InitPacketManager();

	m_uiManager = new CUIManager;
	m_playManager = new CPlayManager;

	m_hwnd = _hwnd;
	m_gameState = INTRO;

	m_uiManager->InitUIManager();
	m_playManager->InitPlayerManager();

	SetTimer(m_hwnd, 1, 1, NULL);

	return false;
}

void CGameManager::ExitGameManager()
{
	KillTimer(m_hwnd, 1);
	CNetworkManager::getInstance().ExitNetworkManager();
	CPacketManager::getInstance().ExitPacketManager();
}

void CGameManager::Timer(HWND _hwnd)
{
	switch (m_gameState)
	{
	case PLAY:
		// 게임을 진행중일 땐 화면을 계속 지워줌
		InvalidateRect(_hwnd, NULL, TRUE);
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

void CGameManager::EnterGame()
{
	m_gameState = PLAY;
	m_playManager->EnterGame();
}

// 플레이어 이동 시 받은 패킷에 대해 처리
void CGameManager::SetPositionScale(char* _data, int _size)
{
	m_playManager->SetPositionScale(_data, _size);
}
