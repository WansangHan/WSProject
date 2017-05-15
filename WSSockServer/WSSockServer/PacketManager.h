#pragma once
#include <list>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <functional>
#include <concurrent_queue.h>
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
	CBaseSocket* sock;
	char* data;
	int dataSize;
	void SetVal(CBaseSocket* _sock, char* _data, int _dataSize)
	{
		sock = _sock;
		data = _data;
		dataSize = _dataSize;
	}
	PacketInfo() { ZeroMemory(this, sizeof(PacketInfo)); }
};

#pragma pack(1)
struct PacketStructure
{
	SendPacketType packetType;
	int dataSize;
	PacketStructure() { ZeroMemory(this, sizeof(PacketStructure)); }
};

class CPacketManager
{
	static std::unique_ptr<CPacketManager> m_inst;
	static std::once_flag m_once;


	std::thread* th_tcp;


	typedef std::function<void(CBaseSocket*, char*)> Function;
	std::map < RecvPacketType, Function > tcp_function;

	concurrency::concurrent_queue<PacketInfo*> packet_queue_tcp;

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
	void DEVIDE_PACKET_BUNDLE_TCP(CBaseSocket* sock, char* packet, int packetSize);
	void SendPacketToServer(CBaseSocket* sock, SendPacketType type, std::string str);
};

