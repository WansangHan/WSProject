#include "stdafx.h"
#include "IOCP.h"

std::unique_ptr<CIOCP> CIOCP::m_inst;
std::once_flag CIOCP::m_once;

CIOCP::CIOCP()
{
}


CIOCP::~CIOCP()
{
	CloseServer();
}

bool CIOCP::InitServer()
{
	CLogManager::getInstance().InitLogManager();

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		CLogManager::getInstance().WriteLogMessage("WSAStartup Error", "ERROR", true);
		return false;
	}

	
	return true;
}

void CIOCP::Update()
{
	while (1) { Sleep(1000); }
}

void CIOCP::CloseServer()
{
	WSACleanup();
}