#include "stdafx.h"
#include "CalculateServer.h"
#include <sys/epoll.h>
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "LogManager.h"

std::unique_ptr<CCalculateServer> CCalculateServer::m_inst;
std::once_flag CCalculateServer::m_once;

CCalculateServer::CCalculateServer()
{
}


CCalculateServer::~CCalculateServer()
{
}

// 동기화 서버에서 연산 서버로 접속
void CCalculateServer::InitCalculateServer(const char* _ip, int _tcpPort, int _udpPort, int _epfd)
{
	m_TCPSocket = std::make_shared<CTCPSocket>();
	m_UDPSocket = std::make_shared<CUDPSocket>();

	m_TCPSockAddr = std::make_shared<sockaddr_in>();
	m_UDPSockAddr = std::make_shared<sockaddr_in>();

	memset(m_TCPSockAddr.get(), 0, sizeof(m_TCPSockAddr.get()));
	m_TCPSockAddr->sin_family = AF_INET;
	m_TCPSockAddr->sin_addr.s_addr = inet_addr(_ip);
	m_TCPSockAddr->sin_port = htons(_tcpPort);

	// CalculateServer에 TCP 접속
	if (connect(m_TCPSocket.get()->GetSOCKET(), (struct sockaddr*)m_TCPSockAddr.get(), sizeof(*m_TCPSockAddr.get())) == -1)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "CalculaterServer TCP connect() error");
	}

	// EPOLL 오브젝트에 등록 및 Read 대기
	epoll_event event_tcp;
	event_tcp.events = EPOLLIN;
	event_tcp.data.fd = m_TCPSocket->GetSOCKET();

	epoll_ctl(_epfd, EPOLL_CTL_ADD, m_TCPSocket->GetSOCKET(), &event_tcp);

	// 연산 서버에 접속을 알림
	SendToCalculateServer(SendPacketType::SD_SYNCSERVER_ENTER, "", true);

	memset(m_UDPSockAddr.get(), 0, sizeof(m_UDPSockAddr.get()));
	m_UDPSockAddr->sin_family = AF_INET;
	m_UDPSockAddr->sin_addr.s_addr = inet_addr(_ip);
	m_UDPSockAddr->sin_port = htons(_udpPort);

	// CalculateServer에 UDP 접속
	if (connect(m_UDPSocket->GetSOCKET(), (struct sockaddr*)m_UDPSockAddr.get(), sizeof(*m_UDPSockAddr.get())) == -1)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "CalculaterServer UDP connect() error");
	}

	epoll_event event_udp;
	event_udp.events = EPOLLIN;
	event_udp.data.fd = m_UDPSocket->GetSOCKET();

	epoll_ctl(_epfd, EPOLL_CTL_ADD, m_UDPSocket->GetSOCKET(), &event_udp);
}

// 연산 서버에 패킷을 전송하는 함수
void CCalculateServer::SendToCalculateServer(SendPacketType type, std::string str, bool isTCP)
{
	if(isTCP)
		CPacketManager::getInstance().SendPacketToServer(m_TCPSocket, type, str, nullptr, isTCP);
	else
		CPacketManager::getInstance().SendPacketToServer(nullptr, type, str, m_UDPSockAddr.get(), isTCP);
}