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
	delete m_filemanager;
}

bool CLogManager::InitLogManager()
{

	CLogManager::getInstance().WriteLogMessage("INFO", true, "Init LogManager");
	return true;
}

bool CLogManager::WriteLogMessage(char * _level, bool _sendsql, const char * _message, ...)
{
	int len = 0;
	va_list lpStart;
	va_start(lpStart, _message);
	len = _vscprintf(_message, lpStart) + 1;
	std::shared_ptr<char> resMessage = std::shared_ptr<char>(new char[len * sizeof(char)], std::default_delete<char[]>());
	vsprintf_s(resMessage.get(), len, _message, lpStart);

	va_end(lpStart);

	ApplyLogMessage(_level, _sendsql, resMessage.get());


	return true;
}

bool CLogManager::ApplyLogMessage(char * _level, bool _sendsql, const char* _message)
{
	if(_sendsql)
		CCurlManager::getInstance().SendErWnJsonString(_message, _level);

	m_filemanager->WriteFileLog(_message, _level);
	return true;
}
