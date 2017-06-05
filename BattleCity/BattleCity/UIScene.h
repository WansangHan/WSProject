#pragma once
class CUIScene
{
public:
	CUIScene();
	~CUIScene();

	virtual void PrintUIScene(HWND _hwnd, HDC _hdc) = 0;
	virtual void MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam) = 0;
	virtual void CommandHandling(HWND _hwnd, WPARAM _wParam) = 0;
};

