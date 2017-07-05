#pragma once
#include "PacketManager.h"
#include "Player.h"

class CGameManager;

class CPlayManager
{
	CPlayer* m_ownPlayer;
public:
	CPlayManager();
	~CPlayManager();

	void InitPlayerManager();
	void PaintPlay(HWND _hwnd, HDC _hdc);

	void EnterGame();

	CPlayer* GetOwnPlayer() { return m_ownPlayer; }
};

