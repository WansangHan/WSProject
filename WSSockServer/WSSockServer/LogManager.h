#pragma once
#include "SqlManager.h"
#include "FileManager.h"
#include <memory>
#include <thread>
#include <mutex>

class CSqlManager;
class CFileManager;

class CLogManager
{
	static std::unique_ptr<CLogManager> m_inst;
	static std::once_flag m_once;

	CSqlManager* m_sqlmanager;
	CFileManager* m_filemanager;

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

