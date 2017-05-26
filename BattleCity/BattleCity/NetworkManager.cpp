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
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "socket() error");

	memset(&m_TCPSockAddr, 0, sizeof(m_TCPSockAddr));
	m_TCPSockAddr.sin_family = AF_INET;
	//m_TCPSockAddr.sin_addr.s_addr = inet_addr("192.168.127.128");
	//m_TCPSockAddr.sin_port = htons(22222);
	m_TCPSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_TCPSockAddr.sin_port = htons(9999);

	if (connect(m_TCPSocket, (SOCKADDR*)&m_TCPSockAddr, sizeof(m_TCPSockAddr)) == SOCKET_ERROR)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "connect() error");

	isContinue = true;

	m_RecvThread = new std::thread([this] {this->RecvThreadFunction(); });
	return true;
}

void CNetworkManager::ExitNetworkManager()
{
	closesocket(m_TCPSocket);
	isContinue = false;
	m_RecvThread->join();
	delete m_RecvThread;
}

void CNetworkManager::RecvThreadFunction()
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
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "read() error");
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
				CLogManager::getInstance().WriteLogMessage("ERROR", true, "recv SOCKET_ERROR");
				break;
			}
			memcpy(tempBuf.get() + totalBufSize, TempRecvBuffer.get(), socketRemainBuffer);
			RecvBuffer = tempBuf;
			totalBufSize += socketRemainBuffer;
			CLogManager::getInstance().WriteLogMessage("INFO", true, "Packet Link Size : %d", totalBufSize);
		}

		CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE_TCP(RecvBuffer.get(), totalBufSize);
	}
}

bool CNetworkManager::SendToServer(const char* data, int dataSize)
{
	send(m_TCPSocket, data, dataSize, NULL);
	return true;
}
