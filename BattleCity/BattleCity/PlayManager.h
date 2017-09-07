#pragma once
#include "PacketManager.h"
#include "Player.h"

class CGameManager;

class CPlayManager
{
	CPlayer* m_ownPlayer;		// Ŭ���̾�Ʈ�� �÷����ϴ� CPlayer Ŭ���� ����
public:
	CPlayManager();
	~CPlayManager();

	void InitPlayerManager();
	void PaintPlay(HWND _hwnd, HDC _hdc);

	void EnterGame();
	void SetPositionScale(char* _data, int _size);

	CPlayer* GetOwnPlayer() { return m_ownPlayer; }
};

