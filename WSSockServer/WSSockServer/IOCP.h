#ifndef IOCP_H
#define IOCP_H
#include <mutex>
#include <winsock2.h>
#include <mswsock.h>
#include "Overlapped.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

class CIOCP
{
	static std::unique_ptr<CIOCP> m_inst;
	static std::once_flag m_once;

	WSADATA m_wsaData;
	// IOCP 오브젝트
	HANDLE m_CompPort;

	// AcceptEx 오브젝트
	LPFN_ACCEPTEX lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs;
	// LPFN_DISCONNECTEX lpfnDisconnectEx;

	// Send Recv시 필요한 플래그, 변수들
	DWORD AcceptRecvDwBytes;
	DWORD ReadRecvFlag;
	DWORD WritedwFlags;

	// Listen 소켓
	std::shared_ptr<CTCPSocket> m_listenTCPSocket;
	std::shared_ptr<CUDPSocket> m_listenUDPSocket;

	// listen 소켓 address
	sockaddr_in m_listenTCPSocketAddr;
	sockaddr_in m_listenUDPSocketAddr;

	// 임시 클라이언트 UDP sockaddr
	sockaddr_in m_clientUDPSockAddr;
	//

	// IOCP WorkerThread
	std::thread** GetSocketCallbackThread;

	CIOCP();
public:
	static CIOCP& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CIOCP()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CIOCP();

	void WorkerThread();

	// Post~~ : IOCP 등록
	void PostAccept();
	void PostDisconnect(std::shared_ptr<CBaseSocket> sock);
	void PostRead(std::shared_ptr<CBaseSocket> sock, bool isTCP);
	int PostSend(void* buf, int len, std::shared_ptr<CBaseSocket> sock, sockaddr_in* soaddr, bool isTCP);

	// Process~~ : IOCP 이벤트 발생한 오브젝트 후처리
	void ProcessAccept(AcceptOverlapped* ovrlap);
	void ProcessDisconnect(DisconnectOverlapped* ovrlap);
	void ProcessRead(ReadOverlapped* ovrlap, int datalen);
	void ProcessWrite(WriteOverlapped* ovrlap);

	bool InitServer();
	void Update();
	void CloseServer();
};
#endif