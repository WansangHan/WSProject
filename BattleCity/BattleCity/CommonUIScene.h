#ifndef COMMONUISCENE_H
#define COMMONUISCENE_H
#include "UIScene.h"

class CCommonUIScene : public CUIScene
{
	HWND m_xButton;		// ���α׷� ���� ��ư �ڵ�
public:
	CCommonUIScene();
	~CCommonUIScene();

	void PrintUIScene(HWND _hwnd, HDC _hdc);
	void MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam);
	void CommandHandling(HWND _hwnd, WPARAM _wParam);
};
#endif