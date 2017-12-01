#ifndef COMMONUISCENE_H
#define COMMONUISCENE_H
#include "UIScene.h"

class CCommonUIScene : public CUIScene
{
	HWND m_xButton;		// 프로그램 종료 버튼 핸들
public:
	CCommonUIScene();
	~CCommonUIScene();

	void PrintUIScene(HWND _hwnd, HDC _hdc);
	void MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam);
	void CommandHandling(HWND _hwnd, WPARAM _wParam);
};
#endif