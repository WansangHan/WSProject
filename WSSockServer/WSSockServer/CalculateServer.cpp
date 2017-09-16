#include "stdafx.h"
#include "CalculateServer.h"

std::unique_ptr<CCalculateServer> CCalculateServer::m_inst;
std::once_flag CCalculateServer::m_once;

CCalculateServer::CCalculateServer()
{
}


CCalculateServer::~CCalculateServer()
{
}

void CCalculateServer::InitCalculateServer(const char* _ip, int _tcpPort, int _udpPort, HANDLE _compPort, CUDPSocket _udpSocket)
{
	m_TCPSocket = std::make_shared<CTCPSocket>();

	m_TCPSockAddr = std::make_shared<sockaddr_in>();
	m_UDPSockAddr = std::make_shared<sockaddr_in>();

	memset(m_TCPSockAddr.get(), 0, sizeof(m_TCPSockAddr.get()));
	m_TCPSockAddr->sin_family = AF_INET;
	m_TCPSockAddr->sin_addr.s_addr = inet_addr(_ip);
	m_TCPSockAddr->sin_port = htons(_tcpPort);

	if (connect(m_TCPSocket.get()->GetSOCKET(), (SOCKADDR*)m_TCPSockAddr.get(), sizeof(*m_TCPSockAddr.get())) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "CalculaterServer TCP connect() error : %d", WSAGetLastError());
	}

	memset(m_UDPSockAddr.get(), 0, sizeof(m_UDPSockAddr.get()));
	m_UDPSockAddr->sin_family = AF_INET;
	m_UDPSockAddr->sin_addr.s_addr = inet_addr(_ip);
	m_UDPSockAddr->sin_port = htons(_udpPort);

	if (connect(_udpSocket.GetSOCKET(), (SOCKADDR*)m_UDPSockAddr.get(), sizeof(*m_UDPSockAddr.get())) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "CalculaterServer UDP connect() error : %d", WSAGetLastError());
	}
}
