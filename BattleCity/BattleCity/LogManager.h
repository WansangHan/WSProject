#pragma once
#include "FileManager.h"
#include "CurlManager.h"
#include <memory>
#include <thread>
#include <mutex>

class CCurlManager;
class CFileManager;

class CLogManager
{
	static std::unique_ptr<CLogManager> m_inst;
	static std::once_flag m_once;

	CFileManager* m_filemanager;
	CCurlManager* m_curlmanager;

	CLogManager();

	bool ApplyLogMessage(char * _level, bool _sendsql, const char* _message);
public:
	static CLogManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CLogManager()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CLogManager();
	bool InitLogManager();
	bool WriteLogMessage(char* _level, bool _sendsql, const char* _message, ...);
};

