#include "stdafx.h"
#include "LogManager.h"

std::unique_ptr<CLogManager> CLogManager::m_inst;
std::once_flag CLogManager::m_once;

CLogManager::CLogManager()
{
	m_filemanager = new CFileManager;
	InitializeCriticalSection(&cs);
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
	// 가변인자가 문자열에 더해졌을 때를 고려한 문자열 길이를 받음
	len = _vscprintf(_message, lpStart) + 1;
	std::shared_ptr<char> resMessage = std::shared_ptr<char>(new char[len * sizeof(char)], std::default_delete<char[]>());
	// 가변인자가 더해진 문자열을 받음
	vsprintf_s(resMessage.get(), len, _message, lpStart);

	va_end(lpStart);

	ApplyLogMessage(_level, _sendsql, resMessage.get());


	return true;
}

bool CLogManager::ApplyLogMessage(char * _level, bool _sendsql, const char* _message)
{
	EnterCriticalSection(&cs);
	// 데이터 베이스에 저장할 로그라면
	if(_sendsql)
		CCurlManager::getInstance().SendErWnJsonString(_message, _level);
	
	// 파일에 저장
	m_filemanager->WriteFileLog(_message, _level);
	LeaveCriticalSection(&cs);
	return true;
}
