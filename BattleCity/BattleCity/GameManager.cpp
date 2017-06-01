#include "stdafx.h"
#include "GameManager.h"

std::unique_ptr<CGameManager> CGameManager::m_inst;
std::once_flag CGameManager::m_once;

CGameManager::CGameManager()
{
}


CGameManager::~CGameManager()
{
}

bool CGameManager::InitGameManager()
{
	CLogManager::getInstance().InitLogManager();
	CNetworkManager::getInstance().InitNetworkManager();
	CPacketManager::getInstance().InitPacketManager();



	return false;
}

{
}

