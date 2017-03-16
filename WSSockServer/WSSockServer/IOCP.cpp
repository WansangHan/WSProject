#include "stdafx.h"
#include "IOCP.h"


CIOCP::CIOCP()
{
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
}

void CIOCP::Update()
{
	while (1);
}

void CIOCP::CloseServer()
{
	WSACleanup();
}