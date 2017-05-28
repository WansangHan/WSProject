#pragma once

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "LogManager.h"
#include "BaseSocket.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "PacketManager.h"

class CEPOLL
{
	static std::unique_ptr<CEPOLL> m_inst;
	static std::once_flag m_once;

	epoll_event *ep_events;
	int m_epfd;

	std::shared_ptr<CTCPSocket> m_listenTCPSocket;
	std::shared_ptr<CUDPSocket> m_listenUDPSocket;

	sockaddr_in m_listenTCPSocketAddr;
	sockaddr_in m_listenUDPSocketAddr;

	// 임시 클라이언트 UDP sockaddr
	sockaddr_in m_clientUDPSockAddr;
	//

	CEPOLL();
public:
	static CEPOLL& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CEPOLL()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CEPOLL();

	bool InitServer();
	void Update();
	void CloseServer();

	bool SendToClient(void* buf, int len, std::shared_ptr<CBaseSocket> sock, sockaddr_in* soaddr, bool isTCP);
};

