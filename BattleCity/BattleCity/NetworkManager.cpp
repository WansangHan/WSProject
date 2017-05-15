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
		char* message = new char[MAX_SOCKET_BUFFER_SIZE];
		int strLen = recv(m_TCPSocket, message, MAX_SOCKET_BUFFER_SIZE, 0);
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
		char* RecvBuffer = message;

		while (socketRemainBuffer == MAX_SOCKET_BUFFER_SIZE)
		{
			char* tempBuf = new char[totalBufSize + MAX_SOCKET_BUFFER_SIZE];
			memcpy(tempBuf, RecvBuffer, totalBufSize);
			char * TempRecvBuffer = new char[MAX_SOCKET_BUFFER_SIZE];
			socketRemainBuffer = recv(m_TCPSocket, TempRecvBuffer, MAX_SOCKET_BUFFER_SIZE, NULL);
			if (socketRemainBuffer == SOCKET_ERROR)
			{
				CLogManager::getInstance().WriteLogMessage("ERROR", true, "recv SOCKET_ERROR");
				break;
			}
			memcpy(tempBuf + totalBufSize, TempRecvBuffer, socketRemainBuffer);
			RecvBuffer = tempBuf;
			totalBufSize += socketRemainBuffer;
			delete[] TempRecvBuffer;
			CLogManager::getInstance().WriteLogMessage("INFO", true, "Packet Link Size : %d", totalBufSize);
		}

		char* recvData = new char[totalBufSize + 1];
		memcpy(recvData, RecvBuffer, totalBufSize);
		recvData[totalBufSize] = '\0';

		CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE_TCP(recvData, totalBufSize);

		delete[] recvData;
		delete[] message;
	}
}

bool CNetworkManager::SendToServer(const char* data, int dataSize)
{
	send(m_TCPSocket, data, dataSize, NULL);
	return true;
}
