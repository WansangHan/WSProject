#ifndef LOGMANAGER_H
#define LOGMANAGER_H
#include <mutex>
#ifndef IOCP_SERVER
#include <memory>
#endif
#include "FileManager.h"
#include "SqlManager.h"

class CLogManager
{
	static std::unique_ptr<CLogManager> m_inst;
	static std::once_flag m_once;

	CSqlManager* m_sqlmanager;
	CFileManager* m_filemanager;

	std::mutex m_mtx_lock;

	CLogManager();

	// SQL Send 여부를 판단하고 SQL과 File에 로그를 저장하는 함수
	bool ApplyLogMessage(char * _level, bool _sendsql, const char* _message);
public:
	static CLogManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CLogManager()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CLogManager();
	bool InitLogManager();
	// 가변 인자로 넘어온 정수, 문자열 형태의 변수를 로그에 합쳐 ApplyLogMessage 함수를 호출
	bool WriteLogMessage(char* _level, bool _sendsql, const char* _message, ...);
};
#endif