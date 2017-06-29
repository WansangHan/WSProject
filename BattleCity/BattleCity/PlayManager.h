#pragma once
#include "Player.h"
class CPlayManager
{
	CPlayer* m_ownPlayer;
public:
	CPlayManager();
	~CPlayManager();

	void InitPlayerManager();
	void PaintPlay(HWND _hwnd, HDC _hdc);

	CPlayer* GetOwnPlayer() { return m_ownPlayer; }
};

