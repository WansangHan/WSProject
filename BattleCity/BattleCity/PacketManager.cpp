#include "stdafx.h"
#include "PacketManager.h"

std::unique_ptr<CPacketManager> CPacketManager::m_inst;
std::once_flag CPacketManager::m_once;

CPacketManager::CPacketManager()
{
}

void CPacketManager::APPLY_PACKET()
{
	// Queue에 쌓인 Receive TCP 패킷을 처리
	if (!packet_queue_tcp.empty())
	{
		std::shared_ptr<PacketInfo> info;
		packet_queue_tcp.try_pop(info);
		DEVIDE_PACKET_TYPE(info);
	}
	// Queue에 쌓인 Receive UDP 패킷을 처리
	if (!packet_queue_udp.empty())
	{
		std::shared_ptr<PacketInfo> info;
		packet_queue_udp.try_pop(info);
		DEVIDE_PACKET_TYPE(info);
	}
}

void CPacketManager::DEVIDE_PACKET_TYPE(std::shared_ptr<PacketInfo> info)
{
	RecvPacketType packetType = RC_PLAYER_POSITION_SCALE;
	// 패킷 분리
	memcpy(&packetType, info->data.get(), sizeof(RecvPacketType));
	// 함수 포인터 find
	auto it = map_function.find(packetType);
	if (it == map_function.end()) { CLogManager::getInstance().WriteLogMessage("ERROR", true, "map_function.end() : %d", packetType); return; }

	// 패킷 중 protobuffer 영역만 잘라 함수 인자로 전달
	int charSize = info->dataSize - sizeof(int) - sizeof(RecvPacketType);
	std::shared_ptr<char> protoBuf = std::shared_ptr<char>(new char[charSize], std::default_delete<char[]>());
	memcpy(protoBuf.get(), info->data.get() + sizeof(int) + sizeof(RecvPacketType), charSize);
	it->second(protoBuf.get(), charSize);
}

void CPacketManager::InitFunctionmap()
{
	// std::map에 패킷 타입에 따른 함수포인터를 적용하는 부분
	// IOCP -> CLIENT
	map_function.insert(std::make_pair(RC_ENTER_SERVER, std::bind(&CGameManager::EnterPlayer, &CGameManager::getInstance(), std::placeholders::_1, std::placeholders::_2)));
	map_function.insert(std::make_pair(RC_SUCCESS_IOCP_CONNECT, std::bind(&CGameManager::IOCPSuccess, &CGameManager::getInstance(), std::placeholders::_1, std::placeholders::_2)));
	map_function.insert(std::make_pair(RC_SUCCESS_IOCP_UDP, std::bind(&CNetworkManager::SetisIOCPUDPSuccessTrue, &CNetworkManager::getInstance(), std::placeholders::_1, std::placeholders::_2)));
	map_function.insert(std::make_pair(RC_EXIT_PLAYER, std::bind(&CGameManager::ExitPlayer, &CGameManager::getInstance(), std::placeholders::_1, std::placeholders::_2)));
	map_function.insert(std::make_pair(RC_PLAYER_POSITION_SCALE, std::bind(&CGameManager::SetPlayerPositionScale, &CGameManager::getInstance(), std::placeholders::_1, std::placeholders::_2)));
	map_function.insert(std::make_pair(RC_AIOBJECT_POSITION_SCALE, std::bind(&CGameManager::SetAIObjectPositionScale, &CGameManager::getInstance(), std::placeholders::_1, std::placeholders::_2)));
	// EPOLL -> CLIENT
	map_function.insert(std::make_pair(RC_SUCCESS_EPOLL_CONNECT, std::bind(&CGameManager::CompleteConnect, &CGameManager::getInstance(), std::placeholders::_1, std::placeholders::_2)));
	map_function.insert(std::make_pair(RC_SUCCESS_EPOLL_UDP, std::bind(&CNetworkManager::SetisEPOLUDPSuccessTrue, &CNetworkManager::getInstance(), std::placeholders::_1, std::placeholders::_2)));
}

CPacketManager::~CPacketManager()
{
}

void CPacketManager::InitPacketManager()
{
	InitFunctionmap();
	isContinue = true;
}

void CPacketManager::ExitPacketManager()
{
	isContinue = false;
	// Thread의 안전한 종료를 위해 Join
}

void CPacketManager::DEVIDE_PACKET_BUNDLE(char * packet, int packetSize, bool isTCP)
{
	// 패킷이 뭉쳐서 왔을 때 분할해주는 함수
	// 패킷 타입 4Byte / 패킷 사이즈 4Byte / 이후 protobuf 영역
	// 위와 같은 패킷 구조로 되어있고, 패킷 사이즈 4바이트를 받아와 분리한다.
	int curToken = 0;
	int Typesize = 0;
	while (packetSize > curToken)
	{
		Typesize = 0;
		// 사이즈 확인
		memcpy(&Typesize, packet + curToken + 4, sizeof(int));
		std::shared_ptr<char> data = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		// 패킷 분리
		memcpy(data.get(), packet + curToken, Typesize);
		std::shared_ptr<PacketInfo> info = std::make_shared<PacketInfo>();
		info->SetVal(data, Typesize);
		// 패킷 정보를 Queue에 push
		if(isTCP) packet_queue_tcp.push(info);
		else packet_queue_udp.push(info);
		curToken += Typesize;
	}
}

void CPacketManager::SendPacketToServer(SendPacketType type, std::string str, bool isTCP, bool isIOCP)
{
	// 인자로 넘어온 패킷 타입, 패킷을 토대로 패킷 사이즈를 정의한 후 패킷의 형태로 만들어 SendToServer 함수를 호출함
	PacketStructure ps;
	ps.packetType = type;
	ps.dataSize = str.length() + sizeof(PacketStructure);
	std::shared_ptr<char> packet = std::shared_ptr<char>(new char[ps.dataSize], std::default_delete<char[]>());
	// 패킷 헤더인 8바이트 영역에 패킷의 타입과 사이즈를 memcpy
	memcpy(packet.get(), &ps, sizeof(PacketStructure));
	// 이후 영역에 protocolbuffer 형태의 데이터를 이어붙임
	memcpy(packet.get() + sizeof(PacketStructure), str.c_str(), str.length());
	CNetworkManager::getInstance().SendToServer(packet.get(), ps.dataSize, isTCP, isIOCP);
}
