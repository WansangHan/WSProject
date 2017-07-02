#include "stdafx.h"
#include "LogManager.h"

std::unique_ptr<CLogManager> CLogManager::m_inst;
std::once_flag CLogManager::m_once;

CLogManager::CLogManager()
{
	m_sqlmanager = new CSqlManager;
	m_filemanager = new CFileManager;
#ifdef IOCP_SERVER
	InitializeCriticalSection(&cs);
#else
#endif
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
	std::shared_ptr<char> resMessage = std::shared_ptr<char>(new char[len * sizeof(char)], std::default_delete<char[]>());
#ifdef IOCP_SERVER
	vsprintf_s(resMessage.get(), len, _message, lpStart);
#else
	va_start(lpStart, _message);
	vsprintf(resMessage.get(), _message, lpStart);
#endif

	va_end(lpStart);

	ApplyLogMessage(_level, _sendsql, resMessage.get());

	return true;
}

bool CLogManager::ApplyLogMessage(char * _level, bool _sendsql, const char* _message)
{
#ifdef IOCP_SERVER
	EnterCriticalSection(&cs);
#else
	pthread_mutex_lock(&m_mutex);
#endif
	if(_sendsql)
		m_sqlmanager->SendLogMessage(_message, _level);

	m_filemanager->WriteFileLog(_message, _level);
#ifdef IOCP_SERVER
	LeaveCriticalSection(&cs);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
	return true;
}
