#include "stdafx.h"
#include "GameManager.h"


CGameManager::CGameManager()
{
}


CGameManager::~CGameManager()
{
}

bool CGameManager::InitGameManager()
{
	CLogManager::getInstance().InitLogManager();

	CLogManager::getInstance().WriteLogMessage("INFO", true, "Init GameManager");
	return false;
}
