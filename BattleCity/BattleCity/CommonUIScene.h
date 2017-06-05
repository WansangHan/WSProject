#pragma once
#include "UIScene.h"
#include "UIManager.h"

class CCommonUIScene : public CUIScene
{
	HWND m_xButton;
public:
	CCommonUIScene();
	~CCommonUIScene();

	void PrintUIScene(HWND _hwnd, HDC _hdc);
	void MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam);
	void CommandHandling(HWND _hwnd, WPARAM _wParam);
};

