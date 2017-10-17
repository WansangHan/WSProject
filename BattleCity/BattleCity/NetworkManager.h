#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H
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
	// 통신에 쓰일 소켓
	std::shared_ptr<SOCKET> m_IOCP_TCPSocket;
	std::shared_ptr<SOCKET> m_IOCP_UDPSocket;
	std::shared_ptr<SOCKET> m_EPOL_TCPSocket;
	std::shared_ptr<SOCKET> m_EPOL_UDPSocket;
	// 통신에 쓰일 소켓 어드레스
	std::shared_ptr<sockaddr_in> m_IOCP_TCPSockAddr;
	std::shared_ptr<sockaddr_in> m_IOCP_UDPSockAddr;
	std::shared_ptr<sockaddr_in> m_IOCP_ClnSockAddr;
	std::shared_ptr<sockaddr_in> m_EPOL_TCPSockAddr;
	std::shared_ptr<sockaddr_in> m_EPOL_UDPSockAddr;
	std::shared_ptr<sockaddr_in> m_EPOL_ClnSockAddr;
	// Receive를 위한 Thread
	std::unique_ptr<std::thread> m_Recv_IOCP_TCPThread;
	std::unique_ptr<std::thread> m_Recv_IOCP_UDPThread;
	std::unique_ptr<std::thread> m_Recv_EPOL_TCPThread;
	std::unique_ptr<std::thread> m_Recv_EPOL_UDPThread;

	// UDP 정보를 보내기 위한 Thread
	std::unique_ptr<std::thread> m_Apply_Thread;

	// 서버에 UDP 정보가 저장 되었는지
	bool isIOCPUDPSuccess;
	bool isEPOLUDPSuccess;

	bool isContinue;
	CNetworkManager();
public:
	static CNetworkManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CNetworkManager()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CNetworkManager();

	void SetisIOCPUDPSuccessTrue(char * _data, int _size) { isIOCPUDPSuccess = true; }
	void SetisEPOLUDPSuccessTrue(char * _data, int _size) { isEPOLUDPSuccess = true; }
	bool GetisIOCPUDPSuccess() { return isIOCPUDPSuccess; }
	bool GetisEPOLUDPSuccess() { return isEPOLUDPSuccess; }
	bool GetisContinue() { return isContinue; }

	bool InitNetworkManager();
	void ExitNetworkManager();
	// 서버에게 UDP 패킷을 전송하는 함수(서버에 클라이언트의 UDP address 정보를 저장하기 위함)
	void NotifyUDPSocket();
	// 서버와 connect를 진행하는 함수
	void ConnectToServer(SOCKET* _sock, sockaddr_in* _sockAddr, const char* _ip, int _port, bool _isTCP);
	// TCP, UDP Receive 함수
	void RecvTCPThreadFunction(SOCKET* _sock);
	void RecvUDPThreadFunction(SOCKET* _sock, sockaddr_in* _sockAddr);
	// IOCP / EPOLL , TCP, UDP에 따라 Send
	bool SendToServer(const char* data, int dataSize, bool isTCP, bool isIOCP);
};
#endif