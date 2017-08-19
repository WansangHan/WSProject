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

void CGameManager::SetPositionScale(char* _data, int _size)
{
	BattleCity::SetPositionScale RecvData;
	RecvData.ParseFromArray(_data, _size);
	CLogManager::getInstance().WriteLogMessage("INFO", true, "ServerData ID : %d", RecvData._id());
	CLogManager::getInstance().WriteLogMessage("INFO", true, "ServerData VectorX : %f", RecvData._vectorx());
	CLogManager::getInstance().WriteLogMessage("INFO", true, "ServerData VectorY : %f", RecvData._vectory());
	CLogManager::getInstance().WriteLogMessage("INFO", true, "ServerData Scale : %f", RecvData._scale());
}
