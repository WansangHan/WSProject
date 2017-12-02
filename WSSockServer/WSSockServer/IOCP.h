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
	// IOCP ������Ʈ
	HANDLE m_CompPort;

	// AcceptEx ������Ʈ
	LPFN_ACCEPTEX lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs;
	// LPFN_DISCONNECTEX lpfnDisconnectEx;

	// Send Recv�� �ʿ��� �÷���, ������
	DWORD AcceptRecvDwBytes;
	DWORD ReadRecvFlag;
	DWORD WritedwFlags;

	// Listen ����
	std::shared_ptr<CTCPSocket> m_listenTCPSocket;
	std::shared_ptr<CUDPSocket> m_listenUDPSocket;

	// listen ���� address
	sockaddr_in m_listenTCPSocketAddr;
	sockaddr_in m_listenUDPSocketAddr;

	// �ӽ� Ŭ���̾�Ʈ UDP sockaddr
	sockaddr_in m_clientUDPSockAddr;
	//

	// IOCP WorkerThread
	std::thread** GetSocketCallbackThread;

	CIOCP();
public:
	static CIOCP& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CIOCP()); });		// �� ���� ȣ��ǰ� �ϱ� ���� call_once
		return *m_inst;
	}
	~CIOCP();

	void WorkerThread();

	// Post~~ : IOCP ���
	void PostAccept();
	void PostDisconnect(std::shared_ptr<CBaseSocket> sock);
	void PostRead(std::shared_ptr<CBaseSocket> sock, bool isTCP);
	int PostSend(void* buf, int len, std::shared_ptr<CBaseSocket> sock, sockaddr_in* soaddr, bool isTCP);

	// Process~~ : IOCP �̺�Ʈ �߻��� ������Ʈ ��ó��
	void ProcessAccept(AcceptOverlapped* ovrlap);
	void ProcessDisconnect(DisconnectOverlapped* ovrlap);
	void ProcessRead(ReadOverlapped* ovrlap, int datalen);
	void ProcessWrite(WriteOverlapped* ovrlap);

	bool InitServer();
	void Update();
	void CloseServer();
};
#endif