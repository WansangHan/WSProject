#pragma once
#include "UIScene.h"
#include "UIManager.h"

#define MAX_NEWACCOUNT_ID_LENGTH 20
#define MAX_NEWACCOUNT_PW_LENGTH 20
#define MAX_NEWACCOUNT_ML_LENGTH 40

#define MAX_LOGIN_ID_LENGTH 20
#define MAX_LOGIN_PW_LENGTH 20

class CLoginUIScene : public CUIScene
{
	HWND m_NewAccountIDBox;
	HWND m_NewAccountPWBox;
	HWND m_NewAccountMlBox;
	HWND m_NewAccountButton;

	HWND m_loginIDBox;
	HWND m_loginPWBox;
	HWND m_loginButton;
public:
	CLoginUIScene();
	~CLoginUIScene();
	void PrintUIScene(HWND _hwnd, HDC _hdc);
	void MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam);
	void CommandHandling(HWND _hwnd, WPARAM _wParam);
};

