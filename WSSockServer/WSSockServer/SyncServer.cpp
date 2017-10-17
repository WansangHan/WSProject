#include "stdafx.h"
#include "SyncServer.h"

std::unique_ptr<CSyncServer> CSyncServer::m_inst;
std::once_flag CSyncServer::m_once;

CSyncServer::CSyncServer()
{
}


CSyncServer::~CSyncServer()
{
}

void CSyncServer::InitSyncServer()
{

}

// IOCP 서버가 접속했을 때 호출되는 함수
void CSyncServer::EnterSyncServerTCP(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	CLogManager::getInstance().WriteLogMessage("INFO", true, "Enter SyncServer TCP");

	m_TCPSocket = _sock;
}

// IOCP 서버에 패킷을 전송하는 함수
void CSyncServer::SendToSyncServer(SendPacketType type, std::string str, bool isTCP)
{
	if (isTCP)
		CPacketManager::getInstance().SendPacketToServer(m_TCPSocket, type, str, nullptr, isTCP);
	else
		CPacketManager::getInstance().SendPacketToServer(nullptr, type, str, m_UDPSockAddr.get(), isTCP);
}