#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include "LogManager.h"
class CNetworkManager
{
	static std::unique_ptr<CNetworkManager> m_inst;
	static std::once_flag m_once;

	WSADATA m_wsaData;

	SOCKET m_TCPSocket;
	sockaddr_in m_TCPSockAddr;

	CNetworkManager();
public:
	static CNetworkManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CNetworkManager()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CNetworkManager();

	bool InitNetworkManager();
};

