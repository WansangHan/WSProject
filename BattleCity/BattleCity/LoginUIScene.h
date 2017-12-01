#ifndef LOGINUISCENE_H
#define LOGINUISCENE_H
#include "UIScene.h"
// ȸ������ ���� TextBox ���ڿ� ���� ����
#define MAX_NEWACCOUNT_ID_LENGTH 20
#define MAX_NEWACCOUNT_PW_LENGTH 20
#define MAX_NEWACCOUNT_ML_LENGTH 40
// �α��� ���� TextBox ���ڿ� ���� ����
#define MAX_LOGIN_ID_LENGTH 20
#define MAX_LOGIN_PW_LENGTH 20

class CLoginUIScene : public CUIScene
{
	HWND m_NewAccountIDBox;		// ȸ������ ID Box
	HWND m_NewAccountPWBox;		// ȸ������ Password Box
	HWND m_NewAccountMlBox;		// ȸ������ Mail Box
	HWND m_NewAccountButton;	// ȸ������ Button

	HWND m_loginIDBox;		// �α��� ID Box
	HWND m_loginPWBox;		// �α��� Password Box
	HWND m_loginButton;		// �α��� Button
public:
	CLoginUIScene();
	~CLoginUIScene();
	void PrintUIScene(HWND _hwnd, HDC _hdc);
	void MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam);
	void CommandHandling(HWND _hwnd, WPARAM _wParam);
};
#endif