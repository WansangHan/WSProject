#pragma once
#pragma comment(lib, "libprotobufd")
#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include <mswsock.h>
#include "Overlapped.h"
#include "LogManager.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "Lobby.h"
#include "SumTest.pb.h"

class CIOCP
{
	static std::unique_ptr<CIOCP> m_inst;
	static std::once_flag m_once;

	WSADATA m_wsaData;
	HANDLE m_CompPort;

	LPFN_ACCEPTEX lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs;
	// LPFN_DISCONNECTEX lpfnDisconnectEx;

	DWORD AcceptRecvDwBytes;
	DWORD ReadRecvFlag;
	DWORD WritedwFlags;

	CTCPSocket* m_listenTcpSocket;
	CUDPSocket* m_UDPSocket;

	sockaddr_in m_listenSocketAddr;


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

	void PostAccept();
	void PostDisconnect(CBaseSocket* sock);
	void PostRead(CBaseSocket* sock, bool isTCP);
	int PostSend(void* buf, int len, CBaseSocket* sock, sockaddr_in* soaddr, bool isTCP);

	void ProcessAccept(AcceptOverlapped* ovrlap);
	void ProcessDisconnect(DisconnectOverlapped* ovrlap);
	void ProcessRead(ReadOverlapped* ovrlap, int datalen);
	void ProcessWrite(WriteOverlapped* ovrlap);

	bool InitServer();
	void Update();
	void CloseServer();
};

