#ifndef SYNCSERVER_H
#define SYNCSERVER_H
#include "PacketManager.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

enum class SendPacketType : int;

class CSyncServer
{
	static std::unique_ptr<CSyncServer> m_inst;
	static std::once_flag m_once;

	std::shared_ptr<CBaseSocket> m_TCPSocket;

	std::shared_ptr<sockaddr_in> m_UDPSockAddr;

	CSyncServer();
public:
	static CSyncServer& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CSyncServer()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CSyncServer();

	std::shared_ptr<CBaseSocket> GetTCPSocket() { return m_TCPSocket; }

	void InitSyncServer();
	void EnterSyncServerTCP(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
	void SendToSyncServer(SendPacketType type, std::string str, bool isTCP);
};
#endif