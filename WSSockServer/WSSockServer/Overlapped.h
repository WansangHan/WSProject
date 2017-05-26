#pragma once
#include "stdafx.h"
#include "BaseSocket.h"

enum SocketState
{
	ACEP = 1000,
	DISC,
	RECV,
	SEND,
};

struct Overlapped : public WSAOVERLAPPED
{
	std::shared_ptr<CBaseSocket> m_sock;
	sockaddr_in m_addr;
	SocketState m_state;
	Overlapped() {}
	Overlapped(const Overlapped& v)
	{
		this->m_sock = v.m_sock;
		this->m_addr = v.m_addr;
		this->m_state = v.m_state;
	}
};

struct AcceptOverlapped : public Overlapped
{
	struct {
		BYTE m_pLocal[sizeof(SOCKADDR_IN) + 16];
		BYTE m_pRemote[sizeof(SOCKADDR_IN) + 16];
	} m_Bufs;
	AcceptOverlapped() : Overlapped() {
		ZeroMemory(this, sizeof(AcceptOverlapped));
		m_state = ACEP;
	}
	AcceptOverlapped(const AcceptOverlapped& v)
	{
		this->m_Bufs = v.m_Bufs;
	}
};

struct DisconnectOverlapped : public Overlapped
{
	DisconnectOverlapped() {
		ZeroMemory(this, sizeof(DisconnectOverlapped));
		m_state = DISC;
	}
};

struct ReadOverlapped : public Overlapped
{
	bool m_isTCP;
	DWORD m_packetSize;
	WSABUF m_wsabuf;
	char m_buffer[MAX_SOCKET_BUFFER_SIZE];
	ReadOverlapped() {
		ZeroMemory(this, sizeof(ReadOverlapped));
		m_state = RECV;
	}
	ReadOverlapped(const ReadOverlapped& v)
	{
		this->m_isTCP = v.m_isTCP;
		this->m_packetSize = v.m_packetSize;
		this->m_wsabuf = v.m_wsabuf;
		strcpy_s(this->m_buffer, v.m_buffer);
	}
};

struct WriteOverlapped : public Overlapped
{
	bool m_isTCP;
	DWORD m_packetSize;
	WSABUF m_wsabuf;
	char m_buffer[MAX_SOCKET_BUFFER_SIZE];
	WriteOverlapped() {
		ZeroMemory(this, sizeof(WriteOverlapped));
		m_state = SEND;
	}
	WriteOverlapped(const WriteOverlapped& v)
	{
		this->m_isTCP = v.m_isTCP;
		this->m_packetSize = v.m_packetSize;
		this->m_wsabuf = v.m_wsabuf;
		strcpy_s(this->m_buffer, v.m_buffer);
	}
};
