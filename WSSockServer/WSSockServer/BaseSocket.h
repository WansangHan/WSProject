#pragma once
#ifdef IOCP_SERVER
#include <WinSock2.h>
#include <windows.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif
#include "LogManager.h"
class CBaseSocket
{
protected:
#ifdef IOCP_SERVER
	SOCKET m_socket;
#else
	int m_socket;
#endif
public:

	CBaseSocket()
	{
	}

	~CBaseSocket()
	{
	}

	void CloseSocket()
	{
		closesocket(m_socket);
	}

#ifdef IOCP_SERVER
	void SetSOCKET(SOCKET sock) { m_socket = sock; }
	SOCKET GetSOCKET() { return m_socket; }
#else
	void SetSOCKET(int sock) { m_socket = sock; }
	int GetSOCKET() { return m_socket; }
#endif
};

