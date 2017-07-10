#pragma once
#include "PacketManager.h"
#include "Player.h"

class CGameManager;

class CPlayManager
{
	CPlayer* m_ownPlayer;		// 클라이언트가 플레이하는 CPlayer 클래스 변수
public:
	CPlayManager();
	~CPlayManager();

	void InitPlayerManager();
	void PaintPlay(HWND _hwnd, HDC _hdc);

	void EnterGame();

	CPlayer* GetOwnPlayer() { return m_ownPlayer; }
};

