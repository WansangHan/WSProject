#ifndef EPOLL_H
#define EPOLL_H
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <mutex>
#include <memory>
#include "TCPSocket.h"
#include "UDPSocket.h"

class CEPOLL
{
	static std::unique_ptr<CEPOLL> m_inst;
	static std::once_flag m_once;

	// EPOLL Object��
	epoll_event *ep_events;
	int m_epfd;

	// listen ����
	std::shared_ptr<CTCPSocket> m_listenTCPSocket;
	std::shared_ptr<CUDPSocket> m_listenUDPSocket;

	// listen ���� address
	sockaddr_in m_listenTCPSocketAddr;
	sockaddr_in m_listenUDPSocketAddr;

	// �ӽ� Ŭ���̾�Ʈ UDP sockaddr
	sockaddr_in m_clientUDPSockAddr;
	//

	CEPOLL();
public:
	static CEPOLL& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CEPOLL()); });		// �� ���� ȣ��ǰ� �ϱ� ���� call_once
		return *m_inst;
	}
	~CEPOLL();

	bool InitServer();
	void Update();
	void CloseServer();

	bool SendToClient(void* buf, int len, std::shared_ptr<CBaseSocket> sock, sockaddr_in* soaddr, bool isTCP);
};
#endif