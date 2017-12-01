#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H
#include <concurrent_queue.h>
#include "NetworkManager.h"
#include "GameManager.h"

enum RecvPacketType
{
	// IOCP -> CLIENT
	RC_SUCCESS_CALC_TCP = 60000,
	RC_SUCCESS_CALC_UDP,
	// EPOLL -> CLIENT
	RC_ENTER_SYNC_SERVER = 20000,
	RC_SUCCESS_CALC_CONNECT,
	RC_SUCCESS_SYNC_UDP,
	RC_EXIT_PLAYER,
	RC_PLAYER_POSITION_SCALE,
	RC_AIOBJECT_POSITION_SCALE,
	RC_DEATH_NOTIFY,
	RC_INCREASE_SCALE_TO_PLAYER,
};

enum SendPacketType
{
	// CLIENT -> IOCP
	SD_ENTER_CALC_SERVER = 50000,
	SD_APPLY_CALC_UDP_SOCKET,
	// CLIENT -> EPOLL
	SD_ENTER_SYNC_SERVER = 10000,
	SD_APPLY_SYNC_UDP_SOCKET,
	SD_POSITION_SCALE,
};
// Receive한 패킷이 Queue에 enqueue 될 때의 구조체
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
// Send할 패킷 헤더 형태
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

	// 패킷 <-> 함수포인터를 매칭한 map
	typedef std::function<void(char*, int)> Function;
	std::map < RecvPacketType, Function > map_function;

	// Receive한 패킷이 담길 Queue
	concurrency::concurrent_queue<std::shared_ptr<PacketInfo>> packet_queue_tcp;
	concurrency::concurrent_queue<std::shared_ptr<PacketInfo>> packet_queue_udp;

	bool isContinue;

	CPacketManager();

	// 패킷 타입에 따른 함수포인터를 호출하는 함수
	void DEVIDE_PACKET_TYPE(std::shared_ptr<PacketInfo> info);

	// std::map에 패킷 타입에 따른 함수포인터를 적용하는 부분
	void InitFunctionmap();
public:
	static CPacketManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CPacketManager()); });
		return *m_inst;
	}
	~CPacketManager();

	// 패킷 처리하는 함수
	void APPLY_PACKET();

	void InitPacketManager();
	void ExitPacketManager();
	// 뭉쳐서 온 패킷을 분리하는 함수
	void DEVIDE_PACKET_BUNDLE(char* packet, int packetSize, bool isTCP);

	// 받은 protobuffer 패킷 형태를, 미리 정의한 패킷 형태로 만들어 서버로 보내는 함수
	void SendPacketToServer(SendPacketType type, std::string str, bool isTCP, bool isIOCP);
};
#endif