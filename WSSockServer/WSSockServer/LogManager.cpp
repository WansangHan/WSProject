#include "stdafx.h"
#include "LogManager.h"

std::unique_ptr<CLogManager> CLogManager::m_inst;
std::once_flag CLogManager::m_once;

CLogManager::CLogManager()
{
	m_sqlmanager = new CSqlManager;
	m_filemanager = new CFileManager;
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

bool CLogManager::WriteLogMessage(char * _message, char * _level, bool _sendsql)
{
	if(_sendsql)
		m_sqlmanager->SendLogMessage(_message, _level);

	m_filemanager->WriteFileLog(_message, _level);
	return false;
}
