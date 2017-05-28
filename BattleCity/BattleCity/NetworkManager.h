#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <thread>
#include "LogManager.h"
#include "GameManager.h"
#include "PacketManager.h"

class CGameManager;
class CPacketManager;
enum SendPacketType;

class CNetworkManager
{
	static std::unique_ptr<CNetworkManager> m_inst;
	static std::once_flag m_once;

	WSADATA m_wsaData;

	SOCKET m_TCPSocket;
	SOCKET m_UDPSocket;
	sockaddr_in m_TCPSockAddr;
	sockaddr_in m_UDPSockAddr;
	sockaddr_in m_ClnSockAddr;

	std::thread* m_RecvTCPThread;
	std::thread* m_RecvUDPThread;

	bool isContinue;
	CNetworkManager();
public:
	static CNetworkManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CNetworkManager()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CNetworkManager();

	bool InitNetworkManager();
	void ExitNetworkManager();
	void RecvTCPThreadFunction();
	void RecvUDPThreadFunction();
	bool SendToServer(const char* data, int dataSize, bool isTCP);
};

