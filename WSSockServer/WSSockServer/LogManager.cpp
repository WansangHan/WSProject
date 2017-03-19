#include "stdafx.h"
#include "LogManager.h"

std::unique_ptr<CLogManager> CLogManager::m_inst;
std::once_flag CLogManager::m_once;

CLogManager::CLogManager()
{
	m_sqlmanager = new CSqlManager;
}


CLogManager::~CLogManager()
{
}

bool CLogManager::InitLogManager()
{
	if(!m_sqlmanager->InitSQLManager())
		return false;

	return true;
}