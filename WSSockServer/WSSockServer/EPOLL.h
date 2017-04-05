#pragma once

#include "LogManager.h"
class CEPOLL
{
	static std::unique_ptr<CEPOLL> m_inst;
	static std::once_flag m_once;


	CEPOLL();
public:
	static CEPOLL& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CEPOLL()); });		// �� ���� ȣ��ǰ� �ϱ� ���� call_once
		return *m_inst;
	}
	~CEPOLL();

	bool InitServer();
	void Update();
	void CloseServer();
};

