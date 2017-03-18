#include "stdafx.h"
#include "IOCP.h"


CIOCP::CIOCP()
{
	m_sqlmanager = new CSqlManager;
}


CIOCP::~CIOCP()
{
	CloseServer();
}

bool CIOCP::InitServer()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		std::cout << "WSAStartup Error" << std::endl;
		return false;
	}

	m_sqlmanager->InitConnection();
	
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