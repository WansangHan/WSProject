#include "stdafx.h"
#include "IOCP.h"

std::unique_ptr<CIOCP> CIOCP::m_inst;
std::once_flag CIOCP::m_once;

CIOCP::CIOCP()
{
}


CIOCP::~CIOCP()
{
	CloseServer();
}
// IOCP Worker Thread
void CIOCP::WorkerThread()
{
	DWORD dwNoOfBytes = 0;
	ULONG_PTR ulKey = 0;
	Overlapped* ovrlap = nullptr;

	while (true)
	{
		GetQueuedCompletionStatus(m_CompPort, &(DWORD)dwNoOfBytes, &(ULONG_PTR)ulKey, (LPOVERLAPPED*)&ovrlap, INFINITE);

		// 소켓 state에 따라 처리
		switch (ovrlap->m_state)
		{
		case ACEP:
			ProcessAccept((AcceptOverlapped*)ovrlap);
			break;
		case DISC:
			ProcessDisconnect((DisconnectOverlapped*)ovrlap);
			break;
		case RECV:
			ProcessRead((ReadOverlapped*)ovrlap, (int&)(dwNoOfBytes));
			break;
		case SEND:
			ProcessWrite((WriteOverlapped*)ovrlap);
			break;
		default:
			CLogManager::getInstance().WriteLogMessage("INFO", true, "WorkThread switch case Default!!");
			break;
		}
	}
}

// Socket을 Accept 대기 상태로
void CIOCP::PostAccept()
{
	AcceptOverlapped *acceptOverlapped = new AcceptOverlapped;
	std::shared_ptr<CBaseSocket> sock = std::make_shared<CTCPSocket>();
	acceptOverlapped->m_sock = sock;

	if (lpfnAcceptEx(m_listenTCPSocket->GetSOCKET(),
		acceptOverlapped->m_sock->GetSOCKET(),
		&acceptOverlapped->m_Bufs,
		0,
		sizeof(acceptOverlapped->m_Bufs.m_pLocal),
		sizeof(acceptOverlapped->m_Bufs.m_pRemote),
		&AcceptRecvDwBytes, acceptOverlapped) == FALSE)
	{
		DWORD Err = WSAGetLastError();
		if (Err != WSA_IO_PENDING)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "lpfnAcceptEx() Error : %d", Err);
			return;
		}
	}
}

// Socket의 Disconnect 대기 상태로
void CIOCP::PostDisconnect(std::shared_ptr<CBaseSocket> sock)
{
	DisconnectOverlapped* disconnectOverlapped = new DisconnectOverlapped;
	disconnectOverlapped->m_sock = sock;


	if (TransmitFile(disconnectOverlapped->m_sock->GetSOCKET(),
		0, 0, 0, disconnectOverlapped, 0, TF_DISCONNECT | TF_REUSE_SOCKET) == FALSE)
	{
		DWORD Err = WSAGetLastError();
		if (Err != WSA_IO_PENDING)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "TransmitFile() Error : %d", Err);
			return;
		}
	}
}

// Socket을 Read 대기 상태로
void CIOCP::PostRead(std::shared_ptr<CBaseSocket> sock, bool isTCP)
{
	ReadOverlapped* readOverlapped = new ReadOverlapped;
	readOverlapped->m_isTCP = isTCP;

	readOverlapped->m_wsabuf.buf = readOverlapped->m_buffer;
	readOverlapped->m_wsabuf.len = MAX_SOCKET_BUFFER_SIZE;
	// TCP 인지
	if (isTCP)
	{
		readOverlapped->m_sock = sock;
		if (WSARecv(sock->GetSOCKET(), &readOverlapped->m_wsabuf, 1, &readOverlapped->m_packetSize, &ReadRecvFlag, readOverlapped, NULL) == SOCKET_ERROR)		// 데이터 수신
		{
			DWORD Err;
			if ((Err = WSAGetLastError()) != WSA_IO_PENDING)
			{
				CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSARecv Error : %d", Err);
				return;
			}
		}
	}
	else
	{
		readOverlapped->m_sock = m_listenUDPSocket;
		int udpAddrSize = sizeof(sockaddr_in);
		if (WSARecvFrom(readOverlapped->m_sock->GetSOCKET(), &readOverlapped->m_wsabuf, 1, &readOverlapped->m_packetSize, &ReadRecvFlag, (sockaddr*)&readOverlapped->m_addr, &udpAddrSize, readOverlapped, NULL) != 0)
		{
			DWORD Err = WSAGetLastError();
			if (Err != WSA_IO_PENDING) {
				CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSARecvFrom Error : %d", Err);
				return;
			}
		}
	}
}

int CIOCP::PostSend(void* buf, int len, std::shared_ptr<CBaseSocket> sock, sockaddr_in* soaddr, bool isTCP)
{
	WriteOverlapped* writeOverlapped = new WriteOverlapped;

	memcpy(writeOverlapped->m_buffer, buf, len);

	writeOverlapped->m_wsabuf.buf = writeOverlapped->m_buffer;
	writeOverlapped->m_wsabuf.len = len;

	// TCP인지
	if (isTCP)
	{
		writeOverlapped->m_sock = sock;
		if (WSASend(sock->GetSOCKET(), &writeOverlapped->m_wsabuf, 1, &writeOverlapped->m_packetSize, WritedwFlags, writeOverlapped, NULL) == SOCKET_ERROR)
		{
			DWORD Err;
			if ((Err = WSAGetLastError()) != WSA_IO_PENDING)
			{
				CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSASend Error : %d", Err);
				return 0;
			}
		}
	}
	else
	{
		if (WSASendTo(m_listenUDPSocket->GetSOCKET(), &writeOverlapped->m_wsabuf, 1, &writeOverlapped->m_packetSize, WritedwFlags, (SOCKADDR*)&m_clientUDPSockAddr, sizeof(*soaddr), writeOverlapped, NULL) == SOCKET_ERROR)
		{
			DWORD Err;
			if ((Err = WSAGetLastError()) != WSA_IO_PENDING)
			{
				CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSASendTo Error : %d", Err);
				return 0;
			}
		}
	}
	return len;
}

// Client Accept 시 후처리
void CIOCP::ProcessAccept(AcceptOverlapped* ovrlap)
{
	SOCKET so = m_listenTCPSocket->GetSOCKET();
	if (setsockopt(ovrlap->m_sock->GetSOCKET(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		(const char *)&so, sizeof(so)) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "setsockopt so_update_accept_context Error : %d", WSAGetLastError());
		return;
	}

	sockaddr* lpLocalSockAddr = NULL;
	sockaddr* lpRemoteSockAddr = NULL;
	int nLocalSockaddrLen = 0;
	int nRemoteSockaddrLen = 0;
	// AcceptEx 호출로 리턴된 버퍼에서 로컬, 리모트 어드레스를 얻어옴
	lpfnGetAcceptExSockaddrs(
		&((AcceptOverlapped*)ovrlap)->m_Bufs,
		0,
		sizeof(((AcceptOverlapped*)ovrlap)->m_Bufs.m_pLocal),
		sizeof(((AcceptOverlapped*)ovrlap)->m_Bufs.m_pRemote),
		&lpLocalSockAddr,
		&nLocalSockaddrLen,
		&lpRemoteSockAddr,
		&nRemoteSockaddrLen);

	// Accept한 소켓을 IOCP 오브젝트에 넣어줌
	CreateIoCompletionPort((HANDLE)ovrlap->m_sock->GetSOCKET(), m_CompPort, 0, 0);
	// Recv 대기
	PostRead(ovrlap->m_sock, true);
	// 다음 클라이언트 받을 준비
	PostAccept();

	delete ovrlap;

}

// Client Disconnect 시 후처리
void CIOCP::ProcessDisconnect(DisconnectOverlapped* ovrlap)
{
	// 소켓 close
	ovrlap->m_sock->CloseSocket();
	delete ovrlap;
}

void CIOCP::ProcessRead(ReadOverlapped* ovrlap, int datalen)
{
	// 클라이언트의 종료 요청시 PostDisconnect 호출
	if (datalen == 0) { PostDisconnect(ovrlap->m_sock); return; }

	bool isTCP = ovrlap->m_isTCP;
	// 임시 코드
	if (!isTCP)
		m_clientUDPSockAddr = ovrlap->m_addr;
	int addrSize = sizeof(m_listenUDPSocketAddr);
	int socketRemainBuffer = datalen;
	int totalBufSize = datalen;
	std::shared_ptr<char> RecvBuffer = std::shared_ptr<char>(new char[datalen], std::default_delete<char[]>());
	memcpy(RecvBuffer.get(), ovrlap->m_buffer, datalen);

	// 읽은 버퍼의 크기가 최대 버퍼 사이즈와 같다면
	while (socketRemainBuffer == MAX_SOCKET_BUFFER_SIZE)
	{
		std::shared_ptr<char> tempBuf = std::shared_ptr<char>(new char[totalBufSize + MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		memcpy(tempBuf.get(), RecvBuffer.get(), totalBufSize);
		std::shared_ptr<char> TempRecvBuffer = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		// 남은 버퍼를 더 받음
		socketRemainBuffer = recv(ovrlap->m_sock->GetSOCKET(), TempRecvBuffer.get(), MAX_SOCKET_BUFFER_SIZE, NULL);
		if (socketRemainBuffer == SOCKET_ERROR)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", true, "recv() error in linking packet");
			break;
		}
		// 더 받은 버퍼를 기존 버퍼에 이어 붙임
		memcpy(tempBuf.get() + totalBufSize, TempRecvBuffer.get(), socketRemainBuffer);
		RecvBuffer = tempBuf;
		totalBufSize += socketRemainBuffer;
		CLogManager::getInstance().WriteLogMessage("INFO", true, "Packet Link Size : %d", totalBufSize);
	}
	// 패킷 분석 및 적용
	CPacketManager::getInstance().DEVIDE_PACKET_BUNDLE_TCP(ovrlap->m_sock, RecvBuffer, totalBufSize, isTCP);
	// Recv 대기
	PostRead(ovrlap->m_sock, isTCP);
	delete ovrlap;
}

void CIOCP::ProcessWrite(WriteOverlapped* ovrlap)
{
	delete ovrlap;
}

bool CIOCP::InitServer()
{
	CLogManager::getInstance().InitLogManager();

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSAStartup Error");
		return false;
	}
	// IOCP 오브젝트 생성
	m_CompPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_CompPort == NULL)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "CreateIoCompletionPort() Error : %d", GetLastError());
		return false;
	}

	m_listenTCPSocket = std::make_shared<CTCPSocket>();
	m_listenUDPSocket = std::make_shared<CUDPSocket>();
	// TCP 소켓 IOCP 오브젝트에 등록
	CreateIoCompletionPort((HANDLE)m_listenTCPSocket->GetSOCKET(), m_CompPort, 0, 0);

	memset(&m_listenTCPSocketAddr, 0, sizeof(m_listenTCPSocketAddr));
	m_listenTCPSocketAddr.sin_family = AF_INET;
	m_listenTCPSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_listenTCPSocketAddr.sin_port = htons(9999);

	if (::bind(m_listenTCPSocket->GetSOCKET(), (SOCKADDR*)&m_listenTCPSocketAddr, sizeof(m_listenTCPSocketAddr)) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "TCP Bind() Error : %d", WSAGetLastError());
		return false;
	}

	int enable = 0;
	// AcceptEx를 사용하기 위한 소켓 옵션
	if (setsockopt(m_listenTCPSocket->GetSOCKET(), SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (const char*)&enable, sizeof(int)) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "setsockopt SO_CONDITIONAL_ACCEPT Error : %d", WSAGetLastError());
		return false;
	}

	if (listen(m_listenTCPSocket->GetSOCKET(), 1000) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "Listen() Error : %d", WSAGetLastError());
		return false;
	}

	GUID guidAcceptEx = WSAID_ACCEPTEX;
	DWORD acceptRecvDwBytes;
	if (WSAIoctl(
		m_listenTCPSocket->GetSOCKET(),
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx,
		sizeof(guidAcceptEx),
		&lpfnAcceptEx,
		sizeof(lpfnAcceptEx),
		&acceptRecvDwBytes,
		NULL,
		NULL
	) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSAIoctl(get AcceptEx) Error: %d", WSAGetLastError());
		return false;
	}

	GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	DWORD getAcceptexSockaddrsRecvDwBytes;
	if (WSAIoctl(
		m_listenTCPSocket->GetSOCKET(),
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidGetAcceptExSockaddrs,
		sizeof(GuidGetAcceptExSockaddrs),
		&lpfnGetAcceptExSockaddrs,
		sizeof(lpfnGetAcceptExSockaddrs),
		&getAcceptexSockaddrsRecvDwBytes,
		NULL,
		NULL))
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSAIoctl(get GetAcceptExSockaddr) Error: %d", WSAGetLastError());
		return false;
	}

	memset(&m_listenUDPSocketAddr, 0, sizeof(SOCKADDR_IN));
	m_listenUDPSocketAddr.sin_family = PF_INET;
	m_listenUDPSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_listenUDPSocketAddr.sin_port = htons(8888);

	if (::bind(m_listenUDPSocket->GetSOCKET(), (SOCKADDR*)&m_listenUDPSocketAddr, sizeof(m_listenUDPSocketAddr)) == SOCKET_ERROR)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", true, "UDP Bind() Error : %d", WSAGetLastError());
		return false;
	}

	CreateIoCompletionPort((HANDLE)m_listenUDPSocket->GetSOCKET(), m_CompPort, 0, 0);

	GetSocketCallbackThread = new std::thread*[WORKERTHREAD_NUM];
	for (int i = 0; i < WORKERTHREAD_NUM; i++)
	{
		GetSocketCallbackThread[i] = new std::thread([this] {this->WorkerThread(); });
	}
	CPacketManager::getInstance().InitPacketManager();

	PostAccept();
	PostRead(nullptr, false);
	
	return true;
}

void CIOCP::Update()
{
	while (1) { Sleep(1000); }
}

void CIOCP::CloseServer()
{
	for (int i = 0; i < WORKERTHREAD_NUM; i++)
	{
		delete GetSocketCallbackThread[i];
	}

	delete[] GetSocketCallbackThread;
	WSACleanup();
}