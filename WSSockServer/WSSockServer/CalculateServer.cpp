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

// IOCP �������� EPOLL ������ ����
void CCalculateServer::InitCalculateServer(const char* _ip, int _tcpPort, int _udpPort, HANDLE _compPort)
{
	m_TCPSocket = std::make_shared<CTCPSocket>();
	m_UDPSocket = std::make_shared<CUDPSocket>();

	m_TCPSockAddr = std::make_shared<sockaddr_in>();
	m_UDPSockAddr = std::make_shared<sockaddr_in>();

	memset(m_TCPSockAddr.get(), 0, sizeof(m_TCPSockAddr.get()));
	m_TCPSockAddr->sin_family = AF_INET;
	m_TCPSockAddr->sin_addr.s_addr = inet_addr(_ip);
	m_TCPSockAddr->sin_port = htons(_tcpPort);

	// CalculateServer�� TCP ����
	if (connect(m_TCPSocket.get()->GetSOCKET(), (SOCKADDR*)m_TCPSockAddr.get(), sizeof(*m_TCPSockAddr.get())) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "CalculaterServer TCP connect() error : %d", WSAGetLastError());
	}

	// IOCP ������Ʈ�� ��� �� Read ���
	CreateIoCompletionPort((HANDLE)m_TCPSocket->GetSOCKET(), _compPort, 0, 0);
	CIOCP::getInstance().PostRead(m_TCPSocket, true);

	// EPOLL ������ IOCP ���� ������ �˸�
	SendToCalculateServer(SendPacketType::SD_SYNCSERVER_ENTER, "", true);

	memset(m_UDPSockAddr.get(), 0, sizeof(m_UDPSockAddr.get()));
	m_UDPSockAddr->sin_family = AF_INET;
	m_UDPSockAddr->sin_addr.s_addr = inet_addr(_ip);
	m_UDPSockAddr->sin_port = htons(_udpPort);

	// CalculateServer�� UDP ����
	if (connect(m_UDPSocket->GetSOCKET(), (SOCKADDR*)m_UDPSockAddr.get(), sizeof(*m_UDPSockAddr.get())) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "CalculaterServer UDP connect() error : %d", WSAGetLastError());
	}

	CreateIoCompletionPort((HANDLE)m_UDPSocket->GetSOCKET(), _compPort, 0, 0);
}

// EPOLL ������ ��Ŷ�� �����ϴ� �Լ�
void CCalculateServer::SendToCalculateServer(SendPacketType type, std::string str, bool isTCP)
{
	if(isTCP)
		CPacketManager::getInstance().SendPacketToServer(m_TCPSocket, type, str, nullptr, isTCP);
	else
		CPacketManager::getInstance().SendPacketToServer(nullptr, type, str, m_UDPSockAddr.get(), isTCP);
}