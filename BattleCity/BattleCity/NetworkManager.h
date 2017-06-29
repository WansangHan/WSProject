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

	SOCKET* m_IOCP_TCPSocket;
	SOCKET* m_IOCP_UDPSocket;
	SOCKET* m_EPOL_TCPSocket;
	SOCKET* m_EPOL_UDPSocket;

	sockaddr_in* m_IOCP_TCPSockAddr;
	sockaddr_in* m_IOCP_UDPSockAddr;
	sockaddr_in* m_IOCP_ClnSockAddr;
	sockaddr_in* m_EPOL_TCPSockAddr;
	sockaddr_in* m_EPOL_UDPSockAddr;
	sockaddr_in* m_EPOL_ClnSockAddr;

	std::thread* m_Recv_IOCP_TCPThread;
	std::thread* m_Recv_IOCP_UDPThread;
	std::thread* m_Recv_EPOL_TCPThread;
	std::thread* m_Recv_EPOL_UDPThread;

	bool isContinue;
	CNetworkManager();
public:
	static CNetworkManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CNetworkManager()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CNetworkManager();

	bool GetisContinue() { return isContinue; }

	bool InitNetworkManager();
	void ExitNetworkManager();

	void ConnectToServer(SOCKET* _sock, sockaddr_in* _sockAddr, const char* _ip, int _port, bool _isTCP);

	void RecvTCPThreadFunction(SOCKET* _sock);
	void RecvUDPThreadFunction(SOCKET* _sock, sockaddr_in* _sockAddr);
	bool SendToServer(const char* data, int dataSize, bool isTCP, bool isIOCP);
};

