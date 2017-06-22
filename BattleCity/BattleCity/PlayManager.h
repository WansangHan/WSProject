#pragma once
#include "Player.h"
class CPlayManager
{
	CPlayer* m_ownPlayer;
public:
	CPlayManager();
	~CPlayManager();

	void InitPlayerManager();

	CPlayer* GetOwnPlayer() { return m_ownPlayer; }
};

