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
	// ��ſ� ���� ����
	std::shared_ptr<SOCKET> m_IOCP_TCPSocket;
	std::shared_ptr<SOCKET> m_IOCP_UDPSocket;
	std::shared_ptr<SOCKET> m_EPOL_TCPSocket;
	std::shared_ptr<SOCKET> m_EPOL_UDPSocket;
	// ��ſ� ���� ���� ��巹��
	std::shared_ptr<sockaddr_in> m_IOCP_TCPSockAddr;
	std::shared_ptr<sockaddr_in> m_IOCP_UDPSockAddr;
	std::shared_ptr<sockaddr_in> m_IOCP_ClnSockAddr;
	std::shared_ptr<sockaddr_in> m_EPOL_TCPSockAddr;
	std::shared_ptr<sockaddr_in> m_EPOL_UDPSockAddr;
	std::shared_ptr<sockaddr_in> m_EPOL_ClnSockAddr;
	// Receive�� ���� Thread
	std::unique_ptr<std::thread> m_Recv_IOCP_TCPThread;
	std::unique_ptr<std::thread> m_Recv_IOCP_UDPThread;
	std::unique_ptr<std::thread> m_Recv_EPOL_TCPThread;
	std::unique_ptr<std::thread> m_Recv_EPOL_UDPThread;

	bool isContinue;
	CNetworkManager();
public:
	static CNetworkManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CNetworkManager()); });		// �� ���� ȣ��ǰ� �ϱ� ���� call_once
		return *m_inst;
	}
	~CNetworkManager();

	bool GetisContinue() { return isContinue; }

	bool InitNetworkManager();
	void ExitNetworkManager();
	// ������ connect�� �����ϴ� �Լ�
	void ConnectToServer(SOCKET* _sock, sockaddr_in* _sockAddr, const char* _ip, int _port, bool _isTCP);
	// TCP, UDP Receive �Լ�
	void RecvTCPThreadFunction(SOCKET* _sock);
	void RecvUDPThreadFunction(SOCKET* _sock, sockaddr_in* _sockAddr);
	// IOCP / EPOLL , TCP, UDP�� ���� Send
	bool SendToServer(const char* data, int dataSize, bool isTCP, bool isIOCP);
};

