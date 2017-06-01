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
	RecvPacketType packetType = RECV_SUM_INT;
	memcpy(&packetType, info->data.get(), sizeof(RecvPacketType));
	auto it = map_function.find(packetType);
	if (it == map_function.end()) { CLogManager::getInstance().WriteLogMessage("ERROR", true, "map_function.end() : %d", packetType); return; }

	std::shared_ptr<char> protoBuf = std::shared_ptr<char>(new char[info->dataSize - sizeof(int) - sizeof(RecvPacketType)], std::default_delete<char[]>());
	memcpy(protoBuf.get(), info->data.get() + sizeof(int) + sizeof(RecvPacketType), info->dataSize - sizeof(int) - sizeof(RecvPacketType));

	it->second(info->sock, protoBuf.get());
}

void CPacketManager::InitFunctionmap()
{
}

CPacketManager::~CPacketManager()
{
}

void CPacketManager::InitPacketManager()
{
	InitFunctionmap();
	th_tcp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_TCP, this));
	th_udp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_UDP, this));
}

void CPacketManager::DEVIDE_PACKET_BUNDLE_TCP(std::shared_ptr<CBaseSocket> sock, std::shared_ptr<char> packet, int packetSize, bool isTCP)
{
	int curToken = 0;
	int Typesize = 0;
	while (packetSize > curToken)
	{
		Typesize = 0;
		memcpy(&Typesize, packet.get() + curToken + 4, sizeof(int));
		std::shared_ptr<char> data = std::shared_ptr<char>(new char[Typesize], std::default_delete<char[]>());
		memcpy(data.get(), packet.get() + curToken, Typesize);
		std::shared_ptr<PacketInfo> info = std::make_shared<PacketInfo>();
		info->SetVal(sock, data, Typesize);
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
	PacketStructure ps;
	ps.packetType = type;
	ps.dataSize = str.length() + sizeof(PacketStructure);
	std::shared_ptr<char> packet = std::shared_ptr<char>(new char[ps.dataSize], std::default_delete<char[]>());
	memcpy(packet.get(), &ps, sizeof(PacketStructure));
	memcpy(packet.get() + sizeof(PacketStructure), str.c_str(), str.length());
#ifdef IOCP_SERVER
	CIOCP::getInstance().PostSend(packet.get(), ps.dataSize, sock, sockaddr, isTCP);
#else
	CEPOLL::getInstance().SendToClient(packet.get(), ps.dataSize, sock, sockaddr, isTCP);
#endif
}
