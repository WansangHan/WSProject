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
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSAStartup Error");
		return false;
	}

	m_CompPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_CompPort == NULL)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "CreateIoCompletionPort() Error : %d", GetLastError());
		return false;
	}

	m_listenTcpSocket = new CTCPSocket();

	CreateIoCompletionPort((HANDLE)m_listenTcpSocket->GetSOCKET(), m_CompPort, 0, 0);

	memset(&m_listenSocketAddr, 0, sizeof(m_listenSocketAddr));
	m_listenSocketAddr.sin_family = AF_INET;
	m_listenSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_listenSocketAddr.sin_port = htons(9999);

	if (::bind(m_listenTcpSocket->GetSOCKET(), (SOCKADDR*)&m_listenSocketAddr, sizeof(m_listenSocketAddr)) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "TCP Bind() Error : %d", WSAGetLastError());
		return false;
	}

	int enable = 0;
	if (setsockopt(m_listenTcpSocket->GetSOCKET(), SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (const char*)&enable, sizeof(int)) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "setsockopt SO_CONDITIONAL_ACCEPT Error : %d", WSAGetLastError());
		return false;
	}

	if (listen(m_listenTcpSocket->GetSOCKET(), 1000) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "Listen() Error : %d", WSAGetLastError());
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
	delete m_listenTcpSocket;
	WSACleanup();
}