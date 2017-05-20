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

	m_listenTcpSocket = new CTCPSocket();

	memset(&m_listenSocketAddr, 0, sizeof(m_listenSocketAddr));
	m_listenSocketAddr.sin_family = AF_INET;
	m_listenSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_listenSocketAddr.sin_port = htons(22222);

	if (bind(m_listenTcpSocket->GetSOCKET(), (sockaddr*)&m_listenSocketAddr, sizeof(m_listenSocketAddr)) == -1)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "bind() error");
	if (listen(m_listenTcpSocket->GetSOCKET(), 5) == -1)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "listen() error");

	m_epfd = epoll_create(50);
	ep_events = new epoll_event[50];

	epoll_event event;

	event.events = EPOLLIN;
	event.data.fd = m_listenTcpSocket->GetSOCKET();
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listenTcpSocket->GetSOCKET(), &event);

	CPacketManager::getInstance().InitPacketManager();

	return true;
}

void CEPOLL::Update()
{
	while (1)
	{
		int event_cnt = epoll_wait(m_epfd, ep_events, 50, -1);
		if (event_cnt == -1)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "epoll_wait() error");
			break;
		}

		for (int i = 0; i<event_cnt; i++)
		{
			if (ep_events[i].data.fd == m_listenTcpSocket->GetSOCKET())
			{
				socklen_t adr_sz = sizeof(m_listenSocketAddr);
				sockaddr_in clnt_adr;
				int clnt_sock = accept(m_listenTcpSocket->GetSOCKET(), (struct sockaddr*)&clnt_adr, &adr_sz);
				epoll_event event;
				event.events = EPOLLIN;
				event.data.fd = clnt_sock;
				epoll_ctl(m_epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				printf("connected client: %d \n", clnt_sock);
			}
			else
			{
				char* buf = new char[MAX_SOCKET_BUFFER_SIZE];
				int str_len = read(ep_events[i].data.fd, buf, MAX_SOCKET_BUFFER_SIZE);
				if (str_len == 0)    // close request!
				{
					epoll_ctl(
						m_epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
					close(ep_events[i].data.fd);
					printf("closed client: %d \n", ep_events[i].data.fd);
				}
				else
				{
					int socketRemainBuffer = str_len;
					int totalBufSize = str_len;
					char* RecvBuffer = buf;

					while (socketRemainBuffer == MAX_SOCKET_BUFFER_SIZE)
					{
						char* tempBuf = new char[totalBufSize + MAX_SOCKET_BUFFER_SIZE];
						memcpy(tempBuf, RecvBuffer, totalBufSize);
						char * TempRecvBuffer = new char[MAX_SOCKET_BUFFER_SIZE];
						socketRemainBuffer = read(ep_events[i].data.fd, TempRecvBuffer, MAX_SOCKET_BUFFER_SIZE);
						if (socketRemainBuffer == -1)
						{
							CLogManager::getInstance().WriteLogMessage("ERROR", true, "recv : -1");
							break;
						}
						memcpy(tempBuf + totalBufSize, TempRecvBuffer, socketRemainBuffer);
						RecvBuffer = tempBuf;
						totalBufSize += socketRemainBuffer;
						delete[] TempRecvBuffer;
						CLogManager::getInstance().WriteLogMessage("INFO", true, "Packet Link Size : %d", totalBufSize);
					}
					CBaseSocket* sock = new CTCPSocket;
					sock->SetSOCKET(ep_events[i].data.fd);

					CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE_TCP(sock, RecvBuffer, totalBufSize);
				}

			}
		}
	}
}

void CEPOLL::CloseServer()
{
	delete[] ep_events;
	close(m_listenTcpSocket->GetSOCKET());
	close(m_epfd);
}

bool CEPOLL::SendToClient(void * buf, int len, CBaseSocket* sock, sockaddr_in * soaddr, bool isTCP)
{
	write(sock->GetSOCKET(), buf, len);

	return true;
}
