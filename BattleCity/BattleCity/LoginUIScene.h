#ifndef LOGINUISCENE_H
#define LOGINUISCENE_H
#include "UIScene.h"
// 회원가입 관련 TextBox 문자열 제한 길이
#define MAX_NEWACCOUNT_ID_LENGTH 20
#define MAX_NEWACCOUNT_PW_LENGTH 20
#define MAX_NEWACCOUNT_ML_LENGTH 40
// 로그인 관련 TextBox 문자열 제한 길이
#define MAX_LOGIN_ID_LENGTH 20
#define MAX_LOGIN_PW_LENGTH 20

class CLoginUIScene : public CUIScene
{
	HWND m_NewAccountIDBox;		// 회원가입 ID Box
	HWND m_NewAccountPWBox;		// 회원가입 Password Box
	HWND m_NewAccountMlBox;		// 회원가입 Mail Box
	HWND m_NewAccountButton;	// 회원가입 Button

	HWND m_loginIDBox;		// 로그인 ID Box
	HWND m_loginPWBox;		// 로그인 Password Box
	HWND m_loginButton;		// 로그인 Button
public:
	CLoginUIScene();
	~CLoginUIScene();
	void PrintUIScene(HWND _hwnd, HDC _hdc);
	void MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam);
	void CommandHandling(HWND _hwnd, WPARAM _wParam);
};
#endif