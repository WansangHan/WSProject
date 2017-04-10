#pragma once
#include <WinSock2.h>
#include <windows.h>
#include "LogManager.h"
class CTCPSocket
{
	SOCKET m_socket;
public:

	CTCPSocket()
	{
		m_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_socket == INVALID_SOCKET)
		{
			std::cout << "TCP WSASocket() Error : " << WSAGetLastError() << std::endl;
			return;
		}

		int enable = 0;
		if (setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&enable, sizeof(int)) == SOCKET_ERROR)
		{
			std::cout << "setsockopt so_rcvbuf Error : " << WSAGetLastError() << std::endl;
			return;
		}

		enable = 0;
		if (setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&enable, sizeof(int)) == SOCKET_ERROR)
		{
			std::cout << "setsockopt so_sndbuf Error : " << WSAGetLastError() << std::endl;
			return;
		}
	}

	~CTCPSocket()
	{
		closesocket(m_socket);
	}

	void SetSOCKET(SOCKET sock) { m_socket = sock; }
	SOCKET GetSOCKET() { return m_socket; }
};