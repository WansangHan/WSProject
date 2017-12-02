#ifndef BASESOCKET_H
#define BASESOCKET_H
#ifdef IOCP_SERVER
#include <WinSock2.h>
#include <windows.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

// TCP, UDP 소켓의 부모 클래스
class CBaseSocket
{
protected:
#ifdef IOCP_SERVER
	SOCKET m_socket;
#else
	int m_socket;
#endif
public:

	CBaseSocket()
	{
	}

	~CBaseSocket()
	{
	}

	void CloseSocket()
	{
#ifdef IOCP_SERVER
		closesocket(m_socket);
#else
		close(m_socket);
#endif
		
	}

#ifdef IOCP_SERVER
	void SetSOCKET(SOCKET sock) { m_socket = sock; }
	SOCKET GetSOCKET() { return m_socket; }
#else
	void SetSOCKET(int sock) { m_socket = sock; }
	int GetSOCKET() { return m_socket; }
#endif
};
#endif