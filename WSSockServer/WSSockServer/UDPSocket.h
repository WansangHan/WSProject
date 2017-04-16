#pragma once
#include "BaseSocket.h"
class CUDPSocket :
	public CBaseSocket
{
public:

	CUDPSocket()
	{
		m_socket = WSASocket(PF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_socket == INVALID_SOCKET)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "UDP WSASocket() Error : %d", WSAGetLastError());
			return;
		}

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
	}

	~CUDPSocket()
	{
		closesocket(m_socket);
	}
};

