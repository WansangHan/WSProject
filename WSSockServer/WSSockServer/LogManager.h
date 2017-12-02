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