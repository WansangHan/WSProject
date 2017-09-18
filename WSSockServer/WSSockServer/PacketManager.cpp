#include "stdafx.h"
#include "PacketManager.h"

std::unique_ptr<CPacketManager> CPacketManager::m_inst;
std::once_flag CPacketManager::m_once;

CPacketManager::CPacketManager()
{
}

void CPacketManager::APPLY_PACKET_TCP()
{
	while (1)
	{
		// Queue에 쌓인 Receive TCP 패킷을 처리하는 함수
		if (!packet_queue_tcp.empty())
		{
			std::shared_ptr<PacketInfo> info;
#ifdef IOCP_SERVER
			packet_queue_tcp.try_pop(info);
#else
			info = packet_queue_tcp.dequeue();
#endif
			DEVIDE_PACKET_TYPE(info.get());
		}
	}
}

void CPacketManager::APPLY_PACKET_UDP()
{
	while (1)
	{
		// Queue에 쌓인 Receive UDP 패킷을 처리하는 함수
		if (!packet_queue_udp.empty())
		{
			std::shared_ptr<PacketInfo> info;
#ifdef IOCP_SERVER
			packet_queue_udp.try_pop(info);
#else
			info = packet_queue_udp.dequeue();
#endif
			DEVIDE_PACKET_TYPE(info.get());
		}
	}
}

void CPacketManager::DEVIDE_PACKET_TYPE(PacketInfo * info)
{
#ifdef IOCP_SERVER
	RecvPacketType packetType = RecvPacketType::RC_ENTER_SERVER;
#else
	RecvPacketType packetType = RecvPacketType::RC_SYNCSERVER_ENTER;
#endif
	// 패킷 분리
	memcpy(&packetType, info->data.get(), sizeof(RecvPacketType));
	// 함수 포인터 find
	auto it = map_function.find(packetType);
	if (it == map_function.end()) { CLogManager::getInstance().WriteLogMessage("ERROR", true, "map_function.end() : %d", packetType); return; }

	// 패킷 중 protobuffer 영역만 잘라 함수 인자로 전달
	int charSize = info->dataSize - sizeof(int) - sizeof(RecvPacketType);
	std::shared_ptr<char> protoBuf = std::shared_ptr<char>(new char[charSize], std::default_delete<char[]>());
	memcpy(protoBuf.get(), info->data.get() + sizeof(int) + sizeof(RecvPacketType), charSize);

	it->second(info->sock, protoBuf.get(), charSize);
}

void CPacketManager::InitFunctionmap()
{
	// std::map에 패킷 타입에 따른 함수포인터를 적용
#ifdef IOCP_SERVER
	// CLIENT -> IOCP
	map_function.insert(std::make_pair(RecvPacketType::RC_ENTER_SERVER, std::bind(&CInGame::EnterPlayer, &CInGame::getInstance(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	map_function.insert(std::make_pair(RecvPacketType::RC_POSITION_SCALE, std::bind(&CInGame::ApplyPlayerPositionScale, &CInGame::getInstance(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
#else
	// IOCP -> EPOLL
	map_function.insert(std::make_pair(RecvPacketType::RC_SYNCSERVER_ENTER, std::bind(&CSyncServer::EnterSyncServerTCP, &CSyncServer::getInstance(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
#endif
}

CPacketManager::~CPacketManager()
{
}

void CPacketManager::InitPacketManager()
{
	InitFunctionmap();
	// TCP, UDP 패킷 처리하는 Thread 실행
	th_tcp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_TCP, this));
	th_udp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_UDP, this));
}

void CPacketManager::DEVIDE_PACKET_BUNDLE(std::shared_ptr<CBaseSocket> sock, std::shared_ptr<char> packet, int packetSize, bool isTCP)
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
		memcpy(&Typesize, packet.get() + curToken + 4, sizeof(int));
		std::shared_ptr<char> data = std::shared_ptr<char>(new char[Typesize], std::default_delete<char[]>());
		// 패킷 분리
		memcpy(data.get(), packet.get() + curToken, Typesize);
		std::shared_ptr<PacketInfo> info = std::make_shared<PacketInfo>();
		info->SetVal(sock, data, Typesize);
		// 패킷 정보를 Queue에 push
		if (isTCP)
		{
#ifdef IOCP_SERVER
			packet_queue_tcp.push(info);
#else
			packet_queue_tcp.enqueue(info);
#endif
		}
		else
		{
#ifdef IOCP_SERVER
			packet_queue_udp.push(info);
#else
			packet_queue_udp.enqueue(info);
#endif
		}
		curToken += Typesize;
	}
}

void CPacketManager::SendPacketToServer(std::shared_ptr<CBaseSocket> sock, SendPacketType type, std::string str, sockaddr_in* sockaddr, bool isTCP)
{
	// 인자로 넘어온 패킷 타입, 패킷을 토대로 패킷 사이즈를 정의한 후 패킷의 형태로 만들어 Send 함수를 호출함
	PacketStructure ps;
	ps.packetType = type;
	ps.dataSize = str.length() + sizeof(PacketStructure);
	std::shared_ptr<char> packet = std::shared_ptr<char>(new char[ps.dataSize], std::default_delete<char[]>());
	// 패킷 헤더인 8바이트 영역에 패킷의 타입과 사이즈를 memcpy
	memcpy(packet.get(), &ps, sizeof(PacketStructure));
	// 이후 영역에 protocolbuffer 형태의 데이터를 이어붙임
	memcpy(packet.get() + sizeof(PacketStructure), str.c_str(), str.length());
#ifdef IOCP_SERVER
	CIOCP::getInstance().PostSend(packet.get(), ps.dataSize, sock, sockaddr, isTCP);
#else
	CEPOLL::getInstance().SendToClient(packet.get(), ps.dataSize, sock, sockaddr, isTCP);
#endif
}
