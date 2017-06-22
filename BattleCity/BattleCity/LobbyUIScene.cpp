#include "stdafx.h"
#include "LobbyUIScene.h"

extern HINSTANCE hInst;

CLobbyUIScene::CLobbyUIScene()
{
}


CLobbyUIScene::~CLobbyUIScene()
{
}

void CLobbyUIScene::PrintUIScene(HWND _hwnd, HDC _hdc)
{
	m_roomListBox = CreateWindow(TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
		LBS_NOTIFY, 20, 50, 100, 200, _hwnd, (HMENU)LOBBY_ROOM_LISTBOX, hInst, NULL);
}

void CLobbyUIScene::MouseLButtonDownUIScene(HWND _hwnd, LPARAM _lParam)
{
}

void CLobbyUIScene::CommandHandling(HWND _hwnd, WPARAM _wParam)
{
}
