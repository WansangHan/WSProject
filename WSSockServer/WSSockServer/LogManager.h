#pragma once
#include "SqlManager.h"
#include <memory>
#include <thread>
#include <mutex>
class CLogManager
{
	static std::unique_ptr<CLogManager> m_inst;
	static std::once_flag m_once;

	CSqlManager* m_sqlmanager;

	CLogManager();
public:
	static CLogManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CLogManager()); });		// �� ���� ȣ��ǰ� �ϱ� ���� call_once
		return *m_inst;
	}
	~CLogManager();
	bool InitLogManager();
};

