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
			DEVIDE_PACKET_TYPE_TCP(info.get());
		}
	}
}

void CPacketManager::DEVIDE_PACKET_TYPE_TCP(PacketInfo * info)
{
	RecvPacketType packetType = RECV_SUM_INT;
	memcpy(&packetType, info->data.get(), sizeof(RecvPacketType));
	auto it = tcp_function.find(packetType);
	if (it == tcp_function.end()) { CLogManager::getInstance().WriteLogMessage("ERROR", true, "tcp_function.end() : %d", packetType); return; }

	std::shared_ptr<char> protoBuf = std::shared_ptr<char>(new char[info->dataSize - sizeof(int) - sizeof(RecvPacketType)], std::default_delete<char[]>());
	memcpy(protoBuf.get(), info->data.get() + sizeof(int) + sizeof(RecvPacketType), info->dataSize - sizeof(int) - sizeof(RecvPacketType));

	it->second(info->sock, protoBuf.get());
}

void CPacketManager::InitFunctionmap()
{
	tcp_function.insert(std::make_pair(RecvPacketType::RECV_SUM_INT, std::bind(&CLobby::SumData, CLobby::getInstance(), std::placeholders::_1, std::placeholders::_2)));
}

CPacketManager::~CPacketManager()
{
}

void CPacketManager::InitPacketManager()
{
	InitFunctionmap();
	th_tcp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_TCP, this));
}

void CPacketManager::DEVIDE_PACKET_BUNDLE_TCP(std::shared_ptr<CBaseSocket> sock, std::shared_ptr<char> packet, int packetSize)
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
#ifdef IOCP_SERVER
		packet_queue_tcp.push(info);
#else
		packet_queue_tcp.enqueue(info);
#endif
		curToken += Typesize;
	}
}

void CPacketManager::SendPacketToServer(std::shared_ptr<CBaseSocket> sock, SendPacketType type, std::string str)
{
	PacketStructure ps;
	ps.packetType = type;
	ps.dataSize = str.length() + sizeof(PacketStructure);
	std::shared_ptr<char> packet = std::shared_ptr<char>(new char[ps.dataSize], std::default_delete<char[]>());
	memcpy(packet.get(), &ps, sizeof(PacketStructure));
	memcpy(packet.get() + sizeof(PacketStructure), str.c_str(), str.length());
#ifdef IOCP_SERVER
	CIOCP::getInstance().PostSend(packet.get(), ps.dataSize, sock, NULL, true);
#else
	CEPOLL::getInstance().SendToClient(packet, ps.dataSize, sock, NULL, true);
#endif
}
