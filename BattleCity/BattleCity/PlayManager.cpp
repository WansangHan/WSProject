#include "stdafx.h"
#include "PlayManager.h"


CPlayManager::CPlayManager()
{
}


CPlayManager::~CPlayManager()
{
	delete m_ownPlayer;
}

void CPlayManager::InitPlayerManager()
{
	m_ownPlayer = new CPlayer;
}
