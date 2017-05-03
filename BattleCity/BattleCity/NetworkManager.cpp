#include "stdafx.h"
#include "NetworkManager.h"

std::unique_ptr<CNetworkManager> CNetworkManager::m_inst;
std::once_flag CNetworkManager::m_once;

CNetworkManager::CNetworkManager()
{
}


CNetworkManager::~CNetworkManager()
{
}

bool CNetworkManager::InitNetworkManager()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSAStartup() error");

	m_TCPSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(m_TCPSocket == INVALID_SOCKET)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "socket() error");

	memset(&m_TCPSockAddr, 0, sizeof(m_TCPSockAddr));
	m_TCPSockAddr.sin_family = AF_INET;
	m_TCPSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_TCPSockAddr.sin_port = htons(9999);

	if (connect(m_TCPSocket, (SOCKADDR*)&m_TCPSockAddr, sizeof(m_TCPSockAddr)) == SOCKET_ERROR)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "connect() error");

	return true;
}