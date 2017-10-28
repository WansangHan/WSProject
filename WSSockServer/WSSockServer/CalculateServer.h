#ifndef CALCULATESERVER_H
#define CALCULATESERVER_H
#include "PacketManager.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

enum class SendPacketType : int;

class CCalculateServer
{
	static std::unique_ptr<CCalculateServer> m_inst;
	static std::once_flag m_once;

	std::shared_ptr<CBaseSocket> m_TCPSocket;
	std::shared_ptr<CBaseSocket> m_UDPSocket;

	std::shared_ptr<sockaddr_in> m_TCPSockAddr;
	std::shared_ptr<sockaddr_in> m_UDPSockAddr;

	CCalculateServer();
public:
	static CCalculateServer& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CCalculateServer()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CCalculateServer();

	std::shared_ptr<CBaseSocket> GetTCPSocket() { return m_TCPSocket; }

	void InitCalculateServer(const char* _ip, int _tcpPort, int _udpPort, HANDLE _compPort);
	void SendToCalculateServer(SendPacketType type, std::string str, bool isTCP);
};
#endif