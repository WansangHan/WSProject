#include "stdafx.h"
#include "LogManager.h"

std::unique_ptr<CLogManager> CLogManager::m_inst;
std::once_flag CLogManager::m_once;

CLogManager::CLogManager()
{
	m_filemanager = new CFileManager;
}


CLogManager::~CLogManager()
{
}

bool CLogManager::InitLogManager()
{

	return true;
}

bool CLogManager::WriteLogMessage(char * _level, bool _sendsql, const char * _message, ...)
{
	int len = 0;
	va_list lpStart;
	va_start(lpStart, _message);
	len = _vscprintf(_message, lpStart) + 1;
	char* resMessage = new char[len * sizeof(char)];
	vsprintf_s(resMessage, len, _message, lpStart);

	va_end(lpStart);

	ApplyLogMessage(_level, _sendsql, resMessage);

	delete[] resMessage;

	return true;
}

bool CLogManager::ApplyLogMessage(char * _level, bool _sendsql, const char* _message)
{
	m_filemanager->WriteFileLog(_message, _level);
	return true;
}
