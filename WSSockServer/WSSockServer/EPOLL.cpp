#include "stdafx.h"
#include "EPOLL.h"

std::unique_ptr<CEPOLL> CEPOLL::m_inst;
std::once_flag CEPOLL::m_once;

CEPOLL::CEPOLL()
{
}


CEPOLL::~CEPOLL()
{
	CloseServer();
}

bool CEPOLL::InitServer()
{
	CLogManager::getInstance().InitLogManager();

	// TCP 소켓 bind listen
	m_listenTCPSocket = std::make_shared<CTCPSocket>();

	memset(&m_listenTCPSocketAddr, 0, sizeof(m_listenTCPSocketAddr));
	m_listenTCPSocketAddr.sin_family = AF_INET;
	m_listenTCPSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_listenTCPSocketAddr.sin_port = htons(22222);

	if (bind(m_listenTCPSocket->GetSOCKET(), (sockaddr*)&m_listenTCPSocketAddr, sizeof(m_listenTCPSocketAddr)) == -1)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "TCP bind() error");
	if (listen(m_listenTCPSocket->GetSOCKET(), 5) == -1)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "listen() error");

	// UDP 소켓 bind
	m_listenUDPSocket = std::make_shared<CUDPSocket>();

	memset(&m_listenUDPSocketAddr, 0, sizeof(m_listenUDPSocketAddr));
	m_listenUDPSocketAddr.sin_family = AF_INET;
	m_listenUDPSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_listenUDPSocketAddr.sin_port = htons(33333);

	if (bind(m_listenUDPSocket->GetSOCKET(), (sockaddr*)&m_listenUDPSocketAddr, sizeof(m_listenUDPSocketAddr)) == -1)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "UDP bind() error");

	m_epfd = epoll_create(50);
	ep_events = new epoll_event[50];

	epoll_event event;

	// TCP Listen 소켓을 EPOLL 오브젝트에 추가
	event.events = EPOLLIN;
	event.data.fd = m_listenTCPSocket->GetSOCKET();
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listenTCPSocket->GetSOCKET(), &event);

	// UDP Listen 소켓을 EPOLL 오브젝트에 추가
	event.data.fd = m_listenUDPSocket->GetSOCKET();
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listenUDPSocket->GetSOCKET(), &event);

	CPacketManager::getInstance().InitPacketManager();
	CCalculating::getInstance().InitCalculating();

	return true;
}

void CEPOLL::Update()
{
	while (1)
	{
		// 이벤트가 발생한 소켓이 있을 때 까지 대기
		int event_cnt = epoll_wait(m_epfd, ep_events, 50, -1);
		if (event_cnt == -1)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "epoll_wait() error");
			break;
		}

		// 이벤트가 발생한 소켓에 대해 for 처리
		for (int i = 0; i<event_cnt; i++)
		{
			// TCP Listen 소켓에 이벤트가 발생 했을 때(Accept)
			if (ep_events[i].data.fd == m_listenTCPSocket->GetSOCKET())
			{
				socklen_t adr_sz = sizeof(m_listenTCPSocketAddr);
				sockaddr_in clnt_adr;
				// Accept
				int clnt_sock = accept(m_listenTCPSocket->GetSOCKET(), (struct sockaddr*)&clnt_adr, &adr_sz);
				epoll_event event;
				event.events = EPOLLIN;
				event.data.fd = clnt_sock;
				// Accept한 소켓을 EPOLL 오브젝트에 추가
				epoll_ctl(m_epfd, EPOLL_CTL_ADD, clnt_sock, &event);
			}
			else
			{
				std::shared_ptr<char> RecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
				int str_len = 0;
				socklen_t addrSize = sizeof(m_clientUDPSockAddr);
				if (ep_events[i].data.fd != m_listenUDPSocket->GetSOCKET())
					// TCP 소켓에 Receive 이벤트 발생시
					str_len = read(ep_events[i].data.fd, RecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE);
				else
				{
					// UDP 소켓에 Receive 이벤트 발생시
					str_len = recvfrom(ep_events[i].data.fd, RecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, 0, (sockaddr*)&m_clientUDPSockAddr, &addrSize);
					
					std::shared_ptr<CBaseSocket> sock = std::make_shared<CUDPSocket>();
					sock->SetSOCKET(ep_events[i].data.fd);
					// 패킷 분석 및 적용
					CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE(sock, m_clientUDPSockAddr, RecvBuffer, str_len, false);
					continue;
				}
				if (str_len == 0)
				{
					// 클라이언트 종료
					// EPOLL 오브젝트에서 소켓 제거
					epoll_ctl(
						m_epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
					close(ep_events[i].data.fd);
				}
				else
				{
					int socketRemainBuffer = str_len;
					int totalBufSize = str_len;
					// 읽은 버퍼의 크기가 최대 버퍼 사이즈와 같다면
					while (socketRemainBuffer == MAX_SOCKET_BUFFER_SIZE)
					{
						std::shared_ptr<char> tempBuf = std::shared_ptr<char>(new char[totalBufSize + MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
						memcpy(tempBuf.get(), RecvBuffer.get(), totalBufSize);
						std::shared_ptr<char> TempRecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
						// 남은 버퍼를 더 받음
						socketRemainBuffer = read(ep_events[i].data.fd, TempRecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE);
						if (socketRemainBuffer == -1)
						{
							CLogManager::getInstance().WriteLogMessage("ERROR", true, "recv : -1");
							break;
						}
						// 더 받은 버퍼를 기존 버퍼에 이어 붙임
						memcpy(tempBuf.get() + totalBufSize, TempRecvBuffer.get(), socketRemainBuffer);
						RecvBuffer = tempBuf;
						totalBufSize += socketRemainBuffer;
						CLogManager::getInstance().WriteLogMessage("INFO", true, "Packet Link Size : %d", totalBufSize);
					}
					std::shared_ptr<CBaseSocket> sock = std::make_shared<CTCPSocket>();
					sock->SetSOCKET(ep_events[i].data.fd);
					
					// 패킷 분석 및 적용
					CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE(sock, sockaddr_in(), RecvBuffer, totalBufSize, true);
				}

			}
		}
	}
}

void CEPOLL::CloseServer()
{
	delete[] ep_events;
	close(m_listenTCPSocket->GetSOCKET());
	close(m_listenUDPSocket->GetSOCKET());
	close(m_epfd);
}

bool CEPOLL::SendToClient(void* buf, int len, std::shared_ptr<CBaseSocket> sock, sockaddr_in * soaddr, bool isTCP)
{
	// TCP, UDP에 따라 Send
	if (isTCP)
		write(sock->GetSOCKET(), buf, len);
	else
		sendto(m_listenUDPSocket->GetSOCKET(), (char*)buf, len, 0, (sockaddr*)&m_clientUDPSockAddr, sizeof(sockaddr));

	return true;
}
