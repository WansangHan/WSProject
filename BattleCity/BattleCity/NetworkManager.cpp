#include "stdafx.h"
#include "NetworkManager.h"
#include "BattleCity.pb.h"
#include "GameManager.h"
#include "PacketManager.h"
#include "LogManager.h"

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
	isIOCPUDPSuccess = false;
	isEPOLUDPSuccess = false;

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSAStartup() error");

	m_IOCP_TCPSocket = std::make_shared<SOCKET>();
	m_IOCP_UDPSocket = std::make_shared<SOCKET>();
	m_EPOL_TCPSocket = std::make_shared<SOCKET>();
	m_EPOL_UDPSocket = std::make_shared<SOCKET>();

	m_IOCP_TCPSockAddr = std::make_shared<sockaddr_in>();
	m_IOCP_UDPSockAddr = std::make_shared<sockaddr_in>();
	m_IOCP_ClnSockAddr = std::make_shared<sockaddr_in>();
	m_EPOL_TCPSockAddr = std::make_shared<sockaddr_in>();
	m_EPOL_UDPSockAddr = std::make_shared<sockaddr_in>();
	m_EPOL_ClnSockAddr = std::make_shared<sockaddr_in>();

	// IOCP ������ ����
	ConnectToServer(m_IOCP_TCPSocket.get(), m_IOCP_TCPSockAddr.get(), "127.0.0.1", 9999, true);
	ConnectToServer(m_IOCP_UDPSocket.get(), m_IOCP_UDPSockAddr.get(), "127.0.0.1", 8888, false);
	// EPOLL ������ ����
	ConnectToServer(m_EPOL_TCPSocket.get(), m_EPOL_TCPSockAddr.get(), "192.168.68.128", 22222, true);
	ConnectToServer(m_EPOL_UDPSocket.get(), m_EPOL_UDPSockAddr.get(), "192.168.68.128", 33333, false);

	// Receive Thread ���� ����
	isContinue = true;

	// ���ϸ��� Receive �� Thread ����
	m_Recv_IOCP_TCPThread = std::unique_ptr<std::thread>(new std::thread([&]() {RecvTCPThreadFunction(m_IOCP_TCPSocket.get()); }));
	m_Recv_IOCP_UDPThread = std::unique_ptr<std::thread>(new std::thread([&]() {RecvUDPThreadFunction(m_IOCP_UDPSocket.get(), m_IOCP_UDPSockAddr.get()); }));
	m_Recv_EPOL_TCPThread = std::unique_ptr<std::thread>(new std::thread([&]() {RecvTCPThreadFunction(m_EPOL_TCPSocket.get()); }));
	m_Recv_EPOL_UDPThread = std::unique_ptr<std::thread>(new std::thread([&]() {RecvUDPThreadFunction(m_EPOL_UDPSocket.get(), m_EPOL_UDPSockAddr.get()); }));
	return true;
}

void CNetworkManager::ExitNetworkManager()
{
	isContinue = false;
	closesocket(*m_IOCP_TCPSocket.get());
	closesocket(*m_IOCP_UDPSocket.get());
	closesocket(*m_EPOL_TCPSocket.get());
	closesocket(*m_EPOL_UDPSocket.get());
	// �������� ������ ���Ḧ ���� join
	m_Recv_IOCP_TCPThread->join();
	m_Recv_IOCP_UDPThread->join();
	m_Recv_EPOL_TCPThread->join();
	m_Recv_EPOL_UDPThread->join();
	m_Apply_Thread->join();
}

// �������� UDP ������ �����ϱ� ���� Thread ����
void CNetworkManager::NotifyUDPSocket()
{
	m_Apply_Thread = std::unique_ptr<std::thread>(new std::thread([&]()
	{
		BattleCity::ObjectInformation SendData;
		SendData.set__id(CGameManager::getInstance().GetPlayerManagerInstance()->GetOwnPlayer()->GetID());

		while (GetisContinue())
		{
			if (!this->GetisIOCPUDPSuccess())
			{
				CPacketManager::getInstance().SendPacketToServer(SendPacketType::SD_APPLY_CALC_UDP_SOCKET, SendData.SerializeAsString(), false, true);
			}

			if (!this->GetisEPOLUDPSuccess())
			{
				CPacketManager::getInstance().SendPacketToServer(SendPacketType::SD_APPLY_SYNC_UDP_SOCKET, SendData.SerializeAsString(), false, false);
			}

			// �� ���� �� ���� �� ���� ����
			if (this->GetisIOCPUDPSuccess() && this->GetisEPOLUDPSuccess())
				return;
			Sleep(1000);

			CLogManager::getInstance().WriteLogMessage("INFO", true, "UDP Send Retry");
		}
	}));
}

void CNetworkManager::ConnectToServer(SOCKET* _sock, sockaddr_in* _sockAddr, const char* _ip, int _port, bool _isTCP)
{
	// IP, Port�� �޾� �� ������ TCP, UDP ���Ͽ� ������ �� �ֵ��� ��
	if (_isTCP)
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
		if (_isTCP)
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "tcp connect() error");
		else
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "udp connect() error");
	}
}

void CNetworkManager::RecvTCPThreadFunction(SOCKET* _sock)
{
	while (isContinue)
	{
		// Recv
		std::shared_ptr<char> RecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		int strLen = recv(*_sock, RecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, 0);
		if (strLen == -1)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "recv() error");
			return;
		}

		int socketRemainBuffer = strLen;
		int totalBufSize = strLen;

		// ���� ������ ũ�Ⱑ �ִ� ���� ������� ���ٸ�
		while (socketRemainBuffer == MAX_SOCKET_BUFFER_SIZE)
		{
			std::shared_ptr<char> tempBuf = std::shared_ptr<char>(new char[totalBufSize + MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
			memcpy(tempBuf.get(), RecvBuffer.get(), totalBufSize);
			std::shared_ptr<char> TempRecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
			// ���� ���۸� �� ����
			socketRemainBuffer = recv(*_sock, TempRecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, NULL);
			if (socketRemainBuffer == SOCKET_ERROR)
			{
				CLogManager::getInstance().WriteLogMessage("ERROR", true, "recv() error in linking packet");
				break;
			}
			// �� ���� ���۸� ���� ���ۿ� �̾� ����
			memcpy(tempBuf.get() + totalBufSize, TempRecvBuffer.get(), socketRemainBuffer);
			RecvBuffer = tempBuf;
			totalBufSize += socketRemainBuffer;
			CLogManager::getInstance().WriteLogMessage("INFO", true, "Packet Link Size : %d", totalBufSize);
		}
		// ��Ŷ �м� �� ����
		CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE(RecvBuffer.get(), totalBufSize, true);
	}
}

void CNetworkManager::RecvUDPThreadFunction(SOCKET* _sock, sockaddr_in* _sockAddr)
{
	while (isContinue)
	{
		// RecvFrom
		std::shared_ptr<char> RecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		int addrSize = sizeof(*_sockAddr);
		int strLen = recvfrom(*_sock, RecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, 0, (sockaddr*)_sockAddr, &addrSize);
		if (strLen == SOCKET_ERROR)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "recvfrom() error : %d", GetLastError());
			return;
		}
		// ��Ŷ �м� �� ����
		CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE(RecvBuffer.get(), strLen, false);
	}
}

bool CNetworkManager::SendToServer(const char* data, int dataSize, bool isTCP, bool isIOCP)
{
	// IOCP / EPOLL , TCP, UDP�� ���� Send
	if (isIOCP)
	{
		if (isTCP)
		{
			send(*m_IOCP_TCPSocket.get(), data, dataSize, NULL);
		}
		else
		{
			sendto(*m_IOCP_UDPSocket.get(), data, dataSize, NULL, (sockaddr*)m_IOCP_UDPSockAddr.get(), sizeof(*m_IOCP_UDPSockAddr.get()));
		}
	}
	else
	{
		if (isTCP)
		{
			send(*m_EPOL_TCPSocket.get(), data, dataSize, NULL);
		}
		else
		{
			sendto(*m_EPOL_UDPSocket.get(), data, dataSize, NULL, (sockaddr*)m_EPOL_UDPSockAddr.get(), sizeof(*m_EPOL_UDPSockAddr.get()));
		}
	}
	return true;
}
