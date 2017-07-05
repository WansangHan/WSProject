#include "stdafx.h"
#include "PacketManager.h"

std::unique_ptr<CPacketManager> CPacketManager::m_inst;
std::once_flag CPacketManager::m_once;

CPacketManager::CPacketManager()
{
}

void CPacketManager::APPLY_PACKET_TCP()
{
	while (isContinue)
	{
		if (!packet_queue_tcp.empty())
		{
			std::shared_ptr<PacketInfo> info;
			packet_queue_tcp.try_pop(info);
			DEVIDE_PACKET_TYPE(info);
		}
	}
}

void CPacketManager::APPLY_PACKET_UDP()
{
	while (isContinue)
	{
		if (!packet_queue_udp.empty())
		{
			std::shared_ptr<PacketInfo> info;
			packet_queue_udp.try_pop(info);
			DEVIDE_PACKET_TYPE(info);
		}
	}
}

void CPacketManager::DEVIDE_PACKET_TYPE(std::shared_ptr<PacketInfo> info)
{
	RecvPacketType packetType = RC_ENTER_SERVER;
	memcpy(&packetType, info->data.get(), sizeof(RecvPacketType));
	auto it = map_function.find(packetType);
	if (it == map_function.end()) { CLogManager::getInstance().WriteLogMessage("ERROR", true, "map_function.end() : ", packetType); return; }

	std::shared_ptr<char> protoBuf = std::shared_ptr<char>(new char[info->dataSize - sizeof(int) - sizeof(RecvPacketType)], std::default_delete<char[]>());
	memcpy(protoBuf.get(), info->data.get() + sizeof(int) + sizeof(RecvPacketType), info->dataSize - sizeof(int) - sizeof(RecvPacketType));

	it->second(protoBuf.get());
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
	isContinue = true;
	th_tcp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_TCP, this));
	th_udp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_UDP, this));
}

void CPacketManager::ExitPacketManager()
{
	isContinue = false;
	th_tcp->join();
	th_udp->join();
}

void CPacketManager::DEVIDE_PACKET_BUNDLE(char * packet, int packetSize)
{
	int curToken = 0;
	int Typesize = 0;
	while (packetSize > curToken)
	{
		Typesize = 0;
		memcpy(&Typesize, packet + curToken + 4, sizeof(int));
		std::shared_ptr<char> data = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		memcpy(data.get(), packet + curToken, Typesize);
		std::shared_ptr<PacketInfo> info = std::make_shared<PacketInfo>();
		info->SetVal(data, Typesize);
		packet_queue_tcp.push(info);
		curToken += Typesize;
	}
}

void CPacketManager::SendPacketToServer(SendPacketType type, std::string str, bool isTCP, bool isIOCP)
{
	PacketStructure ps;
	ps.packetType = type;
	ps.dataSize = str.length() + sizeof(PacketStructure);
	std::shared_ptr<char> packet = std::shared_ptr<char>(new char[ps.dataSize], std::default_delete<char[]>());
	memcpy(packet.get(), &ps, sizeof(PacketStructure));
	memcpy(packet.get() + sizeof(PacketStructure), str.c_str(), str.length());
	CNetworkManager::getInstance().SendToServer(packet.get(), ps.dataSize, isTCP, isIOCP);
}
