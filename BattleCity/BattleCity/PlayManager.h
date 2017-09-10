#pragma once
#include "PacketManager.h"
#include "Player.h"

class CGameManager;
class CPlayer;

class CPlayManager
{
	CPlayer* m_ownPlayer;		// 클라이언트가 플레이하는 CPlayer 클래스 변수
public:
	CPlayManager();
	~CPlayManager();

	void InitPlayerManager();
	void PaintPlay(HWND _hwnd, HDC _hdc);
	void UpdatePlay();

	void EnterGame();
	void SetPositionScale(char* _data, int _size);
	void CheckKey();

	CPlayer* GetOwnPlayer() { return m_ownPlayer; }
};

