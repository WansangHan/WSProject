#include "stdafx.h"
#include "LoginUIScene.h"

extern HINSTANCE hInst;

CLoginUIScene::CLoginUIScene()
{
}


CLoginUIScene::~CLoginUIScene()
{
}

void CLoginUIScene::PrintUIScene(HWND _hwnd, HDC _hdc)
{
	Rectangle(_hdc, 25, 50, 295, 575);
	TextOut(_hdc, 45, 202, TEXT("아이디"), 3);
	TextOut(_hdc, 45, 252, TEXT("비밀번호"), 4);
	TextOut(_hdc, 45, 302, TEXT("메일"), 2);
	m_NewAccountIDBox = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_AUTOHSCROLL, 120, 200, 150, 25, _hwnd, (HMENU)NEWACCOUNT_ID_BOX, hInst, NULL);
	m_NewAccountPWBox = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_AUTOHSCROLL | ES_PASSWORD, 120, 250, 150, 25, _hwnd, (HMENU)NEWACCOUNT_PW_BOX, hInst, NULL);
	m_NewAccountMlBox = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_AUTOHSCROLL, 120, 300, 150, 25, _hwnd, (HMENU)NEWACCOUNT_ML_BOX, hInst, NULL);
	SendMessage(m_NewAccountIDBox, EM_LIMITTEXT, (WPARAM)MAX_NEWACCOUNT_ID_LENGTH, 0);
	SendMessage(m_NewAccountPWBox, EM_LIMITTEXT, (WPARAM)MAX_NEWACCOUNT_PW_LENGTH, 0);
	SendMessage(m_NewAccountMlBox, EM_LIMITTEXT, (WPARAM)MAX_NEWACCOUNT_ML_LENGTH, 0);
	m_NewAccountButton = CreateWindow(TEXT("button"), TEXT("New Account"), WS_CHILD | WS_VISIBLE |
		BS_PUSHBUTTON, 80, 350, 170, 30, _hwnd, (HMENU)NEWACCOUNT_NEWACCOUNT_BUTTON, hInst, NULL);
	Rectangle(_hdc, 305, 50, 575, 575);
	TextOut(_hdc, 325, 252, TEXT("아이디"), 3);
	TextOut(_hdc, 325, 302, TEXT("비밀번호"), 4);
	m_loginIDBox = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | 
		ES_AUTOHSCROLL, 400, 250, 150, 25, _hwnd, (HMENU)LOGIN_ID_BOX, hInst, NULL);
	m_loginPWBox = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_AUTOHSCROLL | ES_PASSWORD, 400, 300, 150, 25, _hwnd, (HMENU)LOGIN_PW_BOX, hInst, NULL);
	SendMessage(m_loginIDBox, EM_LIMITTEXT, (WPARAM)MAX_LOGIN_ID_LENGTH, 0);
	SendMessage(m_loginPWBox, EM_LIMITTEXT, (WPARAM)MAX_LOGIN_PW_LENGTH, 0);
	m_loginButton = CreateWindow(TEXT("button"), TEXT("Login"), WS_CHILD | WS_VISIBLE |
		BS_PUSHBUTTON, 360, 350, 170, 30, _hwnd, (HMENU)LOGIN_LOGIN_BUTTON, hInst, NULL);
}

void CLoginUIScene::MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam)
{
}

void CLoginUIScene::CommandHandling(HWND _hwnd, WPARAM _wParam)
{
	HMENUNUMBER wmId = (HMENUNUMBER)LOWORD(_wParam);
	switch (wmId)
	{
	case LOGIN_LOGIN_BUTTON:
	{
		TCHAR tid[MAX_LOGIN_ID_LENGTH];
		TCHAR tpw[MAX_LOGIN_PW_LENGTH];
		char cid[MAX_LOGIN_ID_LENGTH];
		char cpw[MAX_LOGIN_PW_LENGTH];
		GetWindowText(m_loginIDBox, tid, MAX_LOGIN_ID_LENGTH);
		GetWindowText(m_loginPWBox, tpw, MAX_LOGIN_PW_LENGTH);
		WideCharToMultiByte(CP_ACP, 0, tid, 128, cid, MAX_LOGIN_ID_LENGTH, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, tpw, 128, cpw, MAX_LOGIN_PW_LENGTH, NULL, NULL);

		Json::Value val = CCurlManager::getInstance().SendLoginJsonString(cid, cpw);
		Json::Value isSuccessJ = val["isSuccess"];
		bool isSuccess = isSuccessJ.asBool();

		if (isSuccess)
		{
			MessageBox(_hwnd, L"Success.", L"회원가입", MB_OK);
		}
		else
		{
			MessageBox(_hwnd, L"fail.", L"회원가입", MB_OK);
		}

	}
	break;
	case NEWACCOUNT_NEWACCOUNT_BUTTON:
	{
		TCHAR tid[MAX_NEWACCOUNT_ID_LENGTH];
		TCHAR tpw[MAX_NEWACCOUNT_PW_LENGTH];
		TCHAR tml[MAX_NEWACCOUNT_ML_LENGTH];
		char cid[MAX_NEWACCOUNT_ID_LENGTH];
		char cpw[MAX_NEWACCOUNT_PW_LENGTH];
		char cml[MAX_NEWACCOUNT_ML_LENGTH];
		GetWindowText(m_NewAccountIDBox, tid, MAX_NEWACCOUNT_ID_LENGTH);
		GetWindowText(m_NewAccountPWBox, tpw, MAX_NEWACCOUNT_PW_LENGTH);
		GetWindowText(m_NewAccountMlBox, tml, MAX_NEWACCOUNT_ML_LENGTH);
		WideCharToMultiByte(CP_ACP, 0, tid, 128, cid, MAX_NEWACCOUNT_ID_LENGTH, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, tpw, 128, cpw, MAX_NEWACCOUNT_PW_LENGTH, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, tml, 128, cml, MAX_NEWACCOUNT_ML_LENGTH, NULL, NULL);

		Json::Value val = CCurlManager::getInstance().SendNewAccountJsonString(cid, cpw, cml);
		Json::Value textJ = val["text"];
		
		CString msg;
		msg.Format(_T("%S"), textJ.asCString());
		MessageBox(_hwnd, msg, L"회원가입", MB_OK);
	}
	break;
	}
}
