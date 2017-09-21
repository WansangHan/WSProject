#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H
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
#include "BattleCity.pb.h"

enum RecvPacketType
{
	RC_ENTER_SERVER = 20000,
	RC_EXIT_PLAYER,
	RC_PLAYER_POSITION_SCALE,
	RC_AIOBJECT_POSITION_SCALE,
};

enum SendPacketType
{
	SD_ENTER_SERVER = 10000,
	SD_POSITION_SCALE,
};
// Receive�� ��Ŷ�� Queue�� enqueue �� ���� ����ü
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
// Send�� ��Ŷ ��� ����
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

	// ��Ŷ <-> �Լ������͸� ��Ī�� map
	typedef std::function<void(char*, int)> Function;
	std::map < RecvPacketType, Function > map_function;

	// Receive�� ��Ŷ�� ��� Queue
	concurrency::concurrent_queue<std::shared_ptr<PacketInfo>> packet_queue_tcp;
	concurrency::concurrent_queue<std::shared_ptr<PacketInfo>> packet_queue_udp;

	bool isContinue;

	CPacketManager();

	// ��Ŷ Ÿ�Կ� ���� �Լ������͸� ȣ���ϴ� �Լ�
	void DEVIDE_PACKET_TYPE(std::shared_ptr<PacketInfo> info);

	// std::map�� ��Ŷ Ÿ�Կ� ���� �Լ������͸� �����ϴ� �κ�
	void InitFunctionmap();
public:
	static CPacketManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CPacketManager()); });
		return *m_inst;
	}
	~CPacketManager();

	// ��Ŷ ó���ϴ� �Լ�
	void APPLY_PACKET();

	void InitPacketManager();
	void ExitPacketManager();
	// ���ļ� �� ��Ŷ�� �и��ϴ� �Լ�
	void DEVIDE_PACKET_BUNDLE(char* packet, int packetSize, bool isTCP);

	// ���� protobuffer ��Ŷ ���¸�, �̸� ������ ��Ŷ ���·� ����� ������ ������ �Լ�
	void SendPacketToServer(SendPacketType type, std::string str, bool isTCP, bool isIOCP);
};
#endif