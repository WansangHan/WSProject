#pragma once
#include <list>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <functional>
#include <Windows.h>
#include <concurrent_queue.h>
#include "LogManager.h"
#include "GameManager.h"

enum RecvPacketType
{
	RECV_SUM_INT = 1000,
};

enum SendPacketType
{
	SEND_SUM_INT = 10000,
};

struct PacketInfo
{
	std::shared_ptr<char> data;
	int dataSize;
	void SetVal(std::shared_ptr<char> _data, int _dataSize)
	{
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


	std::unique_ptr<std::thread> th_tcp;


	typedef std::function<void(char*)> Function;
	std::map < RecvPacketType, Function > tcp_function;

	concurrency::concurrent_queue<std::shared_ptr<PacketInfo>> packet_queue_tcp;

	bool isContinue;

	CPacketManager();

	void APPLY_PACKET_TCP();

	void DEVIDE_PACKET_TYPE_TCP(std::shared_ptr<PacketInfo> info);

	void InitFunctionmap();
public:
	static CPacketManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CPacketManager()); });
		return *m_inst;
	}
	~CPacketManager();

	void InitPacketManager();
	void ExitPacketManager();
	void DEVIDE_PACKET_BUNDLE_TCP(char* packet, int packetSize);

	void SendPacketToServer(SendPacketType type, std::string str);
};

