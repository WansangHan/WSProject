#pragma once
#include <WinSock2.h>
#include <windows.h>
#include "LogManager.h"
class CBaseSocket
{
protected:
	SOCKET m_socket;
public:

	CBaseSocket()
	{
	}

	~CBaseSocket()
	{
	}

	void SetSOCKET(SOCKET sock) { m_socket = sock; }
	SOCKET GetSOCKET() { return m_socket; }
};

