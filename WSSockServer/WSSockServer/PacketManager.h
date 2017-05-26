#pragma once
#include <list>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <functional>
#ifdef IOCP_SERVER
#include <concurrent_queue.h>
#else
#include "SafeQueue.h"
#endif
#include "LogManager.h"
#include "Lobby.h"

enum SendPacketType
{
	SEND_SUM_INT = 1000,
};

enum RecvPacketType
{
	RECV_SUM_INT = 10000,
};

struct PacketInfo
{
	std::shared_ptr<CBaseSocket> sock;
	std::shared_ptr<char> data;
	int dataSize;
	void SetVal(std::shared_ptr<CBaseSocket> _sock, std::shared_ptr<char> _data, int _dataSize)
	{
		sock = _sock;
		data = _data;
		dataSize = _dataSize;
	}
#ifdef IOCP_SERVER
	PacketInfo() { ZeroMemory(this, sizeof(PacketInfo)); }
#else
	PacketInfo() { memset(this, 0, sizeof(PacketInfo)); }
#endif
};

#pragma pack(1)
struct PacketStructure
{
	SendPacketType packetType;
	int dataSize;
#ifdef IOCP_SERVER
	PacketStructure() { ZeroMemory(this, sizeof(PacketStructure)); }
#else
	PacketStructure() { memset(this, 0, sizeof(PacketStructure)); }
#endif
};

class CPacketManager
{
	static std::unique_ptr<CPacketManager> m_inst;
	static std::once_flag m_once;

	std::unique_ptr<std::thread> th_tcp;

	typedef std::function<void(std::shared_ptr<CBaseSocket>, char*)> Function;
	std::map < RecvPacketType, Function > tcp_function;

#ifdef IOCP_SERVER
	concurrency::concurrent_queue<std::shared_ptr<PacketInfo>> packet_queue_tcp;
#else
	CSafeQueue<std::shared_ptr<PacketInfo>> packet_queue_tcp;
#endif

	CPacketManager();

	void APPLY_PACKET_TCP();

	void DEVIDE_PACKET_TYPE_TCP(PacketInfo* info);

	void InitFunctionmap();
public:
	static CPacketManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CPacketManager()); });
		return *m_inst;
	}
	~CPacketManager();

	void InitPacketManager();
	void DEVIDE_PACKET_BUNDLE_TCP(std::shared_ptr<CBaseSocket> sock, std::shared_ptr<char> packet, int packetSize);
	void SendPacketToServer(std::shared_ptr<CBaseSocket> sock, SendPacketType type, std::string str);
};

