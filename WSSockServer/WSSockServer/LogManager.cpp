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

bool CLogManager::WriteLogMessage(char * _level, bool _sendsql, const char * _message, ...)
{
	int len = 0;
	va_list lpStart;
	va_start(lpStart, _message);
#ifdef IOCP_SERVER
	len = _vscprintf(_message, lpStart) + 1;
#else
	len = vsnprintf(NULL, 0, _message, lpStart) + 1;
	va_end(lpStart);
#endif
	std::cout << "len : " << len << std::endl;
	char* resMessage = new char[len * sizeof(char)];
#ifdef IOCP_SERVER
	vsprintf_s(resMessage, len, _message, lpStart);
#else
	va_start(lpStart, _message);
	std::cout << "vsprintf : " << vsprintf(resMessage, _message, lpStart) << std::endl;
#endif

	std::cout << "resMessage : " << resMessage << std::endl;

	va_end(lpStart);

	ApplyLogMessage(_level, _sendsql, resMessage);

	delete[] resMessage;

	return true;
}

bool CLogManager::ApplyLogMessage(char * _level, bool _sendsql, const char* _message)
{
	if(_sendsql)
		m_sqlmanager->SendLogMessage(_message, _level);

	m_filemanager->WriteFileLog(_message, _level);
	return true;
}
