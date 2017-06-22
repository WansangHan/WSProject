#pragma once
#include "UIScene.h"
#include "UIManager.h"
class CLobbyUIScene : public CUIScene
{
	HWND m_roomListBox;
public:
	CLobbyUIScene();
	~CLobbyUIScene();

	void PrintUIScene(HWND _hwnd, HDC _hdc);
	void MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam);
	void CommandHandling(HWND _hwnd, WPARAM _wParam);
};

