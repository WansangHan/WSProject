#include "stdafx.h"
#include "UDPSocket.h"
#include "LogManager.h"

CUDPSocket::CUDPSocket()
{
#ifdef IOCP_SERVER
	m_socket = WSASocket(PF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
	m_socket = socket(PF_INET, SOCK_DGRAM, 0);
#endif
	if (m_socket == INVALID_SOCKET)
	{
#ifdef IOCP_SERVER
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "UDP WSASocket() Error : %d", WSAGetLastError());
#else
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "UDP WSASocket() Error");
#endif
		return;
	}

#ifdef IOCP_SERVER
	int enable = 0;
	if (setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&enable, sizeof(int)) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "setsockopt so_rcvbuf Error : %d", WSAGetLastError());
		return;
	}

	enable = 0;
	if (setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&enable, sizeof(int)) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "setsockopt so_sndbuf Error : %d", WSAGetLastError());
		return;
	}
#endif
}

CUDPSocket::~CUDPSocket()
{
}