#ifndef LOGMANAGER_H
#define LOGMANAGER_H
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
#ifdef IOCP_SERVER
	CRITICAL_SECTION cs;
#else
	pthread_mutex_t m_mutex;
#endif

	// SQL Send ���θ� �Ǵ��ϰ� SQL�� File�� �α׸� �����ϴ� �Լ�
	bool ApplyLogMessage(char * _level, bool _sendsql, const char* _message);
public:
	static CLogManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CLogManager()); });		// �� ���� ȣ��ǰ� �ϱ� ���� call_once
		return *m_inst;
	}
	~CLogManager();
	bool InitLogManager();
	// ���� ���ڷ� �Ѿ�� ����, ���ڿ� ������ ������ �α׿� ���� ApplyLogMessage �Լ��� ȣ��
	bool WriteLogMessage(char* _level, bool _sendsql, const char* _message, ...);
};
#endif