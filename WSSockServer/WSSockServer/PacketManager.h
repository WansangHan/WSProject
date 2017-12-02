#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H
#ifdef IOCP_SERVER
#include <concurrent_queue.h>
#else
#include "SafeQueue.h"
#endif
#include <functional>
#include <map>
#include <thread>
#include "BaseSocket.h"
#ifdef IOCP_SERVER
#include "Calculating.cuh"
#include "SyncServer.h"
#else
#include "InGame.h"
#include "CalculateServer.h"
#endif

enum class SendPacketType : int
{
#ifdef IOCP_SERVER
	// IOCP -> CLIENT
	SD_SUCCESS_CALC_TCP = 60000,
	SD_SUCCESS_CALC_UDP,
	// IOCP -> EPOLL
	SD_AI_STARTING = 40000,
	SD_ENTER_PLAYER_CALC,
	SD_COLLISION_NOTIFY,
#else
	// EPOLL -> CLIENT
	SD_ENTER_SYNC_SERVER = 20000,
	SD_SUCCESS_CALC_CONNECT,
	SD_SUCCESS_SYNC_UDP,
	SD_EXIT_PLAYER,
	SD_PLAYER_POSITION_SCALE,
	SD_AIOBJECT_POSITION_SCALE,
	SD_DEATH_NOTIFY,
	SD_INCREASE_SCALE_TO_PLAYER,
	// EPOLL -> IOCP
	SD_SYNCSERVER_ENTER = 30000,
	SD_MAKE_AIOBJECT,
	SD_ENTER_PEER,
	SD_EXIT_PEER,
	SD_NOTIFY_PLAYER_TRANSFORM,
	SD_INCREASE_SCALE_TO_CALC,
#endif
};

enum class RecvPacketType : int
{
#ifdef IOCP_SERVER
	// CLIENT -> IOCP
	RC_ENTER_CALC_SERVER = 50000,
	RC_APPLY_CALC_UDP_SOCKET,
	// EPOLL -> IOCP
	RC_SYNCSERVER_ENTER = 30000,
	RC_MAKE_AIOBJECT,
	RC_ENTER_PEER,
	RC_EXIT_PEER,
	RC_NOTIFY_PLAYER_TRANSFORM,
	RC_INCREASE_SCALE_TO_CALC,
#else
	// CLIENT -> EPOLL
	RC_ENTER_SYNC_SERVER = 10000,
	RC_APPLY_SYNC_UDP_SOCKET,
	RC_POSITION_SCALE,
	// IOCP -> EPOLL
	RC_AI_STARTING = 40000,
	RC_ENTER_PLAYER_CALC,
	RC_COLLISION_NOTIFY,
#endif
};

struct PacketInfo
{
	std::shared_ptr<CBaseSocket> sock;
	sockaddr_in addr;
	std::shared_ptr<char> data;
	int dataSize;
	void SetVal(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, std::shared_ptr<char> _data, int _dataSize)
	{
		sock = _sock;
		addr = _addr;
		data = _data;
		dataSize = _dataSize;
	}
#ifdef IOCP_SERVER
	PacketInfo() { ZeroMemory(this, sizeof(PacketInfo)); }
#else
	PacketInfo() { memset(this, 0, sizeof(PacketInfo)); }
#endif
};

// ��Ŷ�� ���� �� ��Ŷ ���� ���Ǹ� ���� ����ü
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

	// Queue�� ����� TCP, UDP ��Ŷ���� Dequeue�ϰ� ������ �Լ��� Thread ����
	std::unique_ptr<std::thread> th_tcp;
	std::unique_ptr<std::thread> th_udp;

	// ��Ŷ Ÿ�԰� �׿� ���� �Լ� ȣ�� ���ε��� ���� map
	typedef std::function<void(std::shared_ptr<CBaseSocket>, sockaddr_in, char*, int)> Function;
	std::map < RecvPacketType, Function > map_function;

	// Packet�� ������ ��� ����ü ť(���������� Thread Safe Queue�� ���, ���ۿ��� ������ �ڵ� ���)
#ifdef IOCP_SERVER
	concurrency::concurrent_queue<std::shared_ptr<PacketInfo>> packet_queue_tcp;
	concurrency::concurrent_queue<std::shared_ptr<PacketInfo>> packet_queue_udp;
#else
	CSafeQueue<std::shared_ptr<PacketInfo>> packet_queue_tcp;
	CSafeQueue<std::shared_ptr<PacketInfo>> packet_queue_udp;
#endif

	CPacketManager();

	// Queue�� ����� TCP, UDP ��Ŷ���� Dequeue�� �����ϴ� Thread �Լ�
	void APPLY_PACKET_TCP();
	void APPLY_PACKET_UDP();

	// ��Ŷ Ÿ�԰� ���ε� �� �Լ��� ã�� ȣ���Ű�� �Լ�
	void DEVIDE_PACKET_TYPE(PacketInfo* info);

	// map�� ��Ŷ Ÿ�԰� �Լ��� ���ε��ϴ� �Լ�
	void InitFunctionmap();
public:
	static CPacketManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CPacketManager()); });
		return *m_inst;
	}
	~CPacketManager();

	void InitPacketManager();
	void DEVIDE_PACKET_BUNDLE(std::shared_ptr<CBaseSocket> sock, sockaddr_in addr, std::shared_ptr<char> packet, int packetSize, bool isTCP);
	// Send �Լ�
	void SendPacketToServer(std::shared_ptr<CBaseSocket> sock, SendPacketType type, std::string str, sockaddr_in* sockaddr, bool isTCP);
};
#endif