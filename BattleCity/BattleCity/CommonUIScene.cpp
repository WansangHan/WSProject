#include "stdafx.h"
#include "CommonUIScene.h"
#include "UIManager.h"

extern HINSTANCE hInst;

CCommonUIScene::CCommonUIScene()
{
}


CCommonUIScene::~CCommonUIScene()
{
}

void CCommonUIScene::PrintUIScene(HWND _hwnd, HDC _hdc)
{
	// ���α׷� ���� ��ư ����
	m_xButton = CreateWindow(TEXT("button"), TEXT("X"), WS_CHILD | WS_VISIBLE |
		BS_PUSHBUTTON, 570, 5, 20, 20, _hwnd, (HMENU)COMMON_EXIT_BUTTON, hInst, NULL);
}

void CCommonUIScene::MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam)
{
}

void CCommonUIScene::CommandHandling(HWND _hwnd, WPARAM _wParam)
{
	// ���α׷� ����
	DestroyWindow(_hwnd);
}
