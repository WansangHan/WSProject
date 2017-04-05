#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <iostream>

#include "LogManager.h"

class CIOCP
{
	static std::unique_ptr<CIOCP> m_inst;
	static std::once_flag m_once;

	WSADATA m_wsaData;

	CIOCP();
public:
	static CIOCP& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CIOCP()); });		// �� ���� ȣ��ǰ� �ϱ� ���� call_once
		return *m_inst;
	}
	~CIOCP();

	bool InitServer();
	void Update();
	void CloseServer();
};

