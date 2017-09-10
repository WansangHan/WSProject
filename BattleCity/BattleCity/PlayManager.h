#pragma once
#include "PacketManager.h"
#include "Player.h"

class CGameManager;
class CPlayer;

class CPlayManager
{
	CPlayer* m_ownPlayer;		// Ŭ���̾�Ʈ�� �÷����ϴ� CPlayer Ŭ���� ����
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

