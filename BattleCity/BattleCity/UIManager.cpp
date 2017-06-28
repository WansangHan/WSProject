#include "stdafx.h"
#include "UIManager.h"


CUIManager::CUIManager()
{
}


CUIManager::~CUIManager()
{
	delete m_commonUIScene;
	delete m_loginUIScene;
}

void CUIManager::ChangeScene(HWND _hwnd, SceneState _sceneState)
{
	m_sceneState = _sceneState;
	InvalidateRect(_hwnd, NULL, TRUE);
}

bool CUIManager::InitUIManager()
{
	m_sceneState = LOGIN;

	m_commonUIScene = new CCommonUIScene;
	m_loginUIScene = new CLoginUIScene;

	InitFunction();

	return true;
}

void CUIManager::InitFunction()
{
	Paint_functionmap.insert(std::make_pair(SceneState::LOGIN, std::bind(&CLoginUIScene::PrintUIScene, m_loginUIScene, std::placeholders::_1, std::placeholders::_2)));
	
	CommandHandling_functionmap.insert(std::make_pair(HMENUNUMBER::COMMON_EXIT_BUTTON, std::bind(&CCommonUIScene::CommandHandling, m_commonUIScene, std::placeholders::_1, std::placeholders::_2)));
	CommandHandling_functionmap.insert(std::make_pair(HMENUNUMBER::LOGIN_LOGIN_BUTTON, std::bind(&CLoginUIScene::CommandHandling, m_loginUIScene, std::placeholders::_1, std::placeholders::_2)));
	CommandHandling_functionmap.insert(std::make_pair(HMENUNUMBER::NEWACCOUNT_NEWACCOUNT_BUTTON, std::bind(&CLoginUIScene::CommandHandling, m_loginUIScene, std::placeholders::_1, std::placeholders::_2)));
}

void CUIManager::PaintUI(HWND _hwnd, HDC _hdc)
{
	m_commonUIScene->PrintUIScene(_hwnd, _hdc);
	auto it = Paint_functionmap.find(m_sceneState);
	if (it == Paint_functionmap.end()) { return; }
	it->second(_hwnd, _hdc);
}

void CUIManager::CommandHandling(HWND _hwnd, WPARAM _wParam)
{
	HMENUNUMBER wmId = (HMENUNUMBER)LOWORD(_wParam);
	auto it = CommandHandling_functionmap.find(wmId);
	if (it == CommandHandling_functionmap.end()) { return; }
	it->second(_hwnd, _wParam);
}

void CUIManager::MouceLButtonDown(HWND _hwnd, LPARAM _lParam)
{
	m_commonUIScene->MouseLButtonDownUIScene(_hwnd, _lParam);
	switch (m_sceneState)
	{
	case LOGIN:
		break;
	}
}
