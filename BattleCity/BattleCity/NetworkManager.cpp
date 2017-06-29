#include "stdafx.h"
#include "NetworkManager.h"

std::unique_ptr<CNetworkManager> CNetworkManager::m_inst;
std::once_flag CNetworkManager::m_once;

CNetworkManager::CNetworkManager()
{
}


CNetworkManager::~CNetworkManager()
{
}

bool CNetworkManager::InitNetworkManager()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSAStartup() error");

	m_IOCP_TCPSocket = new SOCKET;
	m_IOCP_UDPSocket = new SOCKET;
	m_EPOL_TCPSocket = new SOCKET;
	m_EPOL_UDPSocket = new SOCKET;

	m_IOCP_TCPSockAddr = new sockaddr_in;
	m_IOCP_UDPSockAddr = new sockaddr_in;
	m_IOCP_ClnSockAddr = new sockaddr_in;
	m_EPOL_TCPSockAddr = new sockaddr_in;
	m_EPOL_UDPSockAddr = new sockaddr_in;
	m_EPOL_ClnSockAddr = new sockaddr_in;

	ConnectToServer(m_IOCP_TCPSocket, m_IOCP_TCPSockAddr, "127.0.0.1", 9999, true);
	ConnectToServer(m_IOCP_UDPSocket, m_IOCP_UDPSockAddr, "127.0.0.1", 8888, false);
	ConnectToServer(m_EPOL_TCPSocket, m_EPOL_TCPSockAddr, "192.168.127.128", 22222, true);
	ConnectToServer(m_EPOL_UDPSocket, m_EPOL_UDPSockAddr, "192.168.127.128", 33333, false);

	isContinue = true;

	m_Recv_IOCP_TCPThread = new std::thread([this] {this->RecvTCPThreadFunction(m_IOCP_TCPSocket); });
	m_Recv_IOCP_UDPThread = new std::thread([this] {this->RecvUDPThreadFunction(m_IOCP_UDPSocket, m_IOCP_ClnSockAddr); });
	m_Recv_EPOL_TCPThread = new std::thread([this] {this->RecvTCPThreadFunction(m_EPOL_TCPSocket); });
	m_Recv_EPOL_UDPThread = new std::thread([this] {this->RecvUDPThreadFunction(m_EPOL_UDPSocket, m_EPOL_ClnSockAddr); });
	return true;
}

void CNetworkManager::ExitNetworkManager()
{
	isContinue = false;
	m_Recv_IOCP_TCPThread->join();
	m_Recv_IOCP_UDPThread->join();
	m_Recv_EPOL_TCPThread->join();
	m_Recv_EPOL_UDPThread->join();
	delete m_Recv_IOCP_TCPThread;
	delete m_Recv_IOCP_UDPThread;
	delete m_Recv_EPOL_TCPThread;
	delete m_Recv_EPOL_UDPThread;
	closesocket(*m_IOCP_TCPSocket);
	closesocket(*m_IOCP_UDPSocket);
	closesocket(*m_EPOL_TCPSocket); 
	closesocket(*m_EPOL_UDPSocket);
	delete m_IOCP_TCPSocket;
	delete m_IOCP_UDPSocket;
	delete m_EPOL_TCPSocket;
	delete m_EPOL_UDPSocket;

	delete m_IOCP_TCPSockAddr;
	delete 	m_IOCP_UDPSockAddr;
	delete 	m_IOCP_ClnSockAddr;
	delete 	m_EPOL_TCPSockAddr;
	delete 	m_EPOL_UDPSockAddr;
	delete 	m_EPOL_ClnSockAddr;
}

void CNetworkManager::ConnectToServer(SOCKET* _sock, sockaddr_in* _sockAddr, const char* _ip, int _port, bool _isTCP)
{
	if(_isTCP)
	{
		*_sock = socket(PF_INET, SOCK_STREAM, 0);
		if (*_sock == INVALID_SOCKET)
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "tcp socket() error");
	}
	else
	{
		*_sock = socket(PF_INET, SOCK_DGRAM, 0);
		if (*_sock == INVALID_SOCKET)
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "udp socket() error");
	}
	

	memset(_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr->sin_family = AF_INET;
	_sockAddr->sin_addr.s_addr = inet_addr(_ip);
	_sockAddr->sin_port = htons(_port);

	if (connect(*_sock, (SOCKADDR*)_sockAddr, sizeof(*_sockAddr)) == SOCKET_ERROR)
	{
		if(_isTCP)
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "tcp connect() error");
		else
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "udp connect() error");
	}
}

void CNetworkManager::RecvTCPThreadFunction(SOCKET* _sock)
{
	while (isContinue)
	{
		std::shared_ptr<char> RecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		int strLen = recv(*_sock, RecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, 0);
		if (strLen == 0)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "Server Disconnect!");
			return;
		}
		else if (strLen == -1)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "recv() error");
			return;
		}

		int socketRemainBuffer = strLen;
		int totalBufSize = strLen;

		while (socketRemainBuffer == MAX_SOCKET_BUFFER_SIZE)
		{
			std::shared_ptr<char> tempBuf = std::shared_ptr<char>(new char[totalBufSize + MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
			memcpy(tempBuf.get(), RecvBuffer.get(), totalBufSize);
			std::shared_ptr<char> TempRecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
			socketRemainBuffer = recv(*_sock, TempRecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, NULL);
			if (socketRemainBuffer == SOCKET_ERROR)
			{
				CLogManager::getInstance().WriteLogMessage("ERROR", true, "recv() error in linking packet");
				break;
			}
			memcpy(tempBuf.get() + totalBufSize, TempRecvBuffer.get(), socketRemainBuffer);
			RecvBuffer = tempBuf;
			totalBufSize += socketRemainBuffer;
			CLogManager::getInstance().WriteLogMessage("INFO", true, "Packet Link Size : %d", totalBufSize);
		}

		CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE(RecvBuffer.get(), totalBufSize);
	}
}

void CNetworkManager::RecvUDPThreadFunction(SOCKET* _sock, sockaddr_in* _sockAddr)
{
	while (isContinue)
	{
		std::shared_ptr<char> RecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		int addrSize = sizeof(*_sockAddr);
		ZeroMemory(_sockAddr, addrSize);
		int strLen = recvfrom(*_sock, RecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, 0, (sockaddr*)_sockAddr, &addrSize);
		if (strLen == SOCKET_ERROR)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "recvfrom() error : %d", GetLastError());
			return;
		}

		CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE(RecvBuffer.get(), strLen);
	}
}

bool CNetworkManager::SendToServer(const char* data, int dataSize, bool isTCP, bool isIOCP)
{
	if (isIOCP)
	{
		if (isTCP)
			send(*m_IOCP_TCPSocket, data, dataSize, NULL);
		else
			sendto(*m_IOCP_UDPSocket, data, dataSize, NULL, (sockaddr*)m_IOCP_UDPSockAddr, sizeof(*m_IOCP_UDPSockAddr));
	}
	else
	{
		if (isTCP)
			send(*m_EPOL_TCPSocket, data, dataSize, NULL);
		else
			sendto(*m_EPOL_UDPSocket, data, dataSize, NULL, (sockaddr*)m_EPOL_UDPSockAddr, sizeof(*m_EPOL_UDPSockAddr));
	}
	return true;
}
