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

	m_TCPSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(m_TCPSocket == INVALID_SOCKET)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "tcp socket() error");

	memset(&m_TCPSockAddr, 0, sizeof(m_TCPSockAddr));
	m_TCPSockAddr.sin_family = AF_INET;
	//m_TCPSockAddr.sin_addr.s_addr = inet_addr("192.168.127.128");
	//m_TCPSockAddr.sin_port = htons(22222);
	m_TCPSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_TCPSockAddr.sin_port = htons(9999);

	if (connect(m_TCPSocket, (SOCKADDR*)&m_TCPSockAddr, sizeof(m_TCPSockAddr)) == SOCKET_ERROR)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "tcp connect() error");

	m_UDPSocket = socket(PF_INET, SOCK_DGRAM, 0);

	if(m_UDPSocket == INVALID_SOCKET)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "udp socket() error");

	memset(&m_UDPSockAddr, 0, sizeof(m_UDPSockAddr));
	m_UDPSockAddr.sin_family = AF_INET;
	//m_UDPSockAddr.sin_addr.s_addr = inet_addr("192.168.127.128");
	//m_UDPSockAddr.sin_port = htons(33333);
	m_UDPSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_UDPSockAddr.sin_port = htons(8888);

	if(connect(m_UDPSocket, (SOCKADDR*)&m_UDPSockAddr, sizeof(m_UDPSockAddr)) == SOCKET_ERROR)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "udp connect() error");

	isContinue = true;

	m_RecvTCPThread = new std::thread([this] {this->RecvTCPThreadFunction(); });
	m_RecvUDPThread = new std::thread([this] {this->RecvUDPThreadFunction(); });
	return true;
}

void CNetworkManager::ExitNetworkManager()
{
	closesocket(m_TCPSocket);
	closesocket(m_UDPSocket);
	isContinue = false;
	m_RecvTCPThread->join();
	m_RecvUDPThread->join();
	delete m_RecvTCPThread;
	delete m_RecvUDPThread;
}

void CNetworkManager::RecvTCPThreadFunction()
{
	while (isContinue)
	{
		std::shared_ptr<char> RecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		int strLen = recv(m_TCPSocket, RecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, 0);
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
			socketRemainBuffer = recv(m_TCPSocket, TempRecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, NULL);
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

void CNetworkManager::RecvUDPThreadFunction()
{
	while (isContinue)
	{
		std::shared_ptr<char> RecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		int addrSize = sizeof(m_ClnSockAddr);
		ZeroMemory(&m_ClnSockAddr, addrSize);
		int strLen = recvfrom(m_UDPSocket, RecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, 0, (sockaddr*)&m_ClnSockAddr, &addrSize);
		if (strLen == SOCKET_ERROR)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "recvfrom() error : %d", GetLastError());
			return;
		}

		CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE(RecvBuffer.get(), strLen);
	}
}

bool CNetworkManager::SendToServer(const char* data, int dataSize, bool isTCP)
{
	if (isTCP)
		send(m_TCPSocket, data, dataSize, NULL);
	else
		sendto(m_UDPSocket, data, dataSize, NULL, (sockaddr*)&m_UDPSockAddr, sizeof(m_UDPSockAddr));
	return true;
}
