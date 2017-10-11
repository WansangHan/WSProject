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
	// 서버와 연결 되어있었다면
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
		// 게임을 진행중일 땐 화면을 계속 지워줌
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
	// 윈도우 창 이동을 위한 코드
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

// 키보드 이벤트 시
void CGameManager::KeyChange()
{
	switch (m_gameState)
	{
	case PLAY:
		m_playManager->CheckKey();
		break;
	}
}

void CGameManager::EnterGame()
{
	m_gameState = LODDING;
	// 서버와 연결
	CNetworkManager::getInstance().InitNetworkManager();
	CPacketManager::getInstance().InitPacketManager();
	isConnected = true;
	m_playManager->EnterGame();
}

// EPOLL 서버에도 접속이 완료 되면, 플레이로 전환
void CGameManager::CompleteConnect(char * _data, int _size)
{
	m_gameState = PLAY;
}

// 플레이어가 들어왔을 때
void CGameManager::EnterPlayer(char * _data, int _size)
{
	m_playManager->EnterPlayer(_data, _size);
}

// 다른 플레이어들이 나갔을 때 처리
void CGameManager::ExitPlayer(char * _data, int _size)
{
	m_playManager->ExitPlayer(_data, _size);
}

// Player Transform 패킷에 대해 처리
void CGameManager::SetPlayerPositionScale(char* _data, int _size)
{
	m_playManager->SetPlayerPositionScale(_data, _size);
}

// AI Transform 패킷에 대해 처리
void CGameManager::SetAIObjectPositionScale(char * _data, int _size)
{
	m_playManager->SetAIObjectPositionScale(_data, _size);
}
