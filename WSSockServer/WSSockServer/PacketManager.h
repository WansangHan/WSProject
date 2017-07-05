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
#include "BaseSocket.h"
#include "InGame.h"
#ifdef IOCP_SERVER
#include "IOCP.h"
#else
#include "EPOLL.h"
#endif
#include "WSSockServer.pb.h"

enum SendPacketType
{
	SD_ENTER_SERVER = 20000,
};

enum RecvPacketType
{
	RC_ENTER_SERVER = 10000,
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
	std::unique_ptr<std::thread> th_udp;

	typedef std::function<void(std::shared_ptr<CBaseSocket>, char*)> Function;
	std::map < RecvPacketType, Function > map_function;

#ifdef IOCP_SERVER
	concurrency::concurrent_queue<std::shared_ptr<PacketInfo>> packet_queue_tcp;
	concurrency::concurrent_queue<std::shared_ptr<PacketInfo>> packet_queue_udp;
#else
	CSafeQueue<std::shared_ptr<PacketInfo>> packet_queue_tcp;
	CSafeQueue<std::shared_ptr<PacketInfo>> packet_queue_udp;
#endif

	CPacketManager();

	void APPLY_PACKET_TCP();
	void APPLY_PACKET_UDP();

	void DEVIDE_PACKET_TYPE(PacketInfo* info);

	void InitFunctionmap();
public:
	static CPacketManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CPacketManager()); });
		return *m_inst;
	}
	~CPacketManager();

	void InitPacketManager();
	void DEVIDE_PACKET_BUNDLE_TCP(std::shared_ptr<CBaseSocket> sock, std::shared_ptr<char> packet, int packetSize, bool isTCP);
	void SendPacketToServer(std::shared_ptr<CBaseSocket> sock, SendPacketType type, std::string str, sockaddr_in* sockaddr, bool isTCP);
};

