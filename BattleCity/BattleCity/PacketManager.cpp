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
			PacketInfo* info;
			packet_queue_tcp.try_pop(info);
			DEVIDE_PACKET_TYPE_TCP(info);
			delete info->data;
			delete info;
		}
	}
}

void CPacketManager::DEVIDE_PACKET_TYPE_TCP(PacketInfo * info)
{
	RecvPacketType packetType = RECV_SUM_INT;
	memcpy(&packetType, info->data, sizeof(RecvPacketType));
	auto it = tcp_function.find(packetType);
	if (it == tcp_function.end()) { CLogManager::getInstance().WriteLogMessage("ERROR", true, "tcp_function.end() : ", packetType); return; }
	char* protoBuf = new char[MAX_SOCKET_BUFFER_SIZE];
	memcpy(protoBuf, info->data + sizeof(int) + sizeof(RecvPacketType), info->dataSize - sizeof(int) - sizeof(RecvPacketType));

	it->second(protoBuf);
	delete protoBuf;
}

void CPacketManager::InitFunctionmap()
{
	tcp_function.insert(std::make_pair(RECV_SUM_INT, std::bind(&CGameManager::SumData, CGameManager::getInstance(), std::placeholders::_1)));
}

CPacketManager::~CPacketManager()
{
}

void CPacketManager::InitPacketManager()
{
	InitFunctionmap();
	isContinue = true;
	th_tcp = new std::thread(&CPacketManager::APPLY_PACKET_TCP, this);
}

void CPacketManager::ExitPacketManager()
{
	isContinue = false;
	th_tcp->join();
	delete th_tcp;
}

void CPacketManager::DEVIDE_PACKET_BUNDLE_TCP(char * packet, int packetSize)
{
	int curToken = 0;
	int Typesize = 0;
	while (packetSize > curToken)
	{
		Typesize = 0;
		memcpy(&Typesize, packet + curToken + 4, sizeof(int));
		char* data = new char[MAX_SOCKET_BUFFER_SIZE];
		memcpy(data, packet + curToken, Typesize);
		PacketInfo* info = new PacketInfo;
		info->SetVal(data, Typesize);
		packet_queue_tcp.push(info);
		curToken += Typesize;
	}
}

void CPacketManager::SendPacketToServer(SendPacketType type, std::string str)
{
	PacketStructure ps;
	ps.packetType = type;
	ps.dataSize = str.length() + sizeof(PacketStructure);
	char* packet = new char[ps.dataSize];
	memcpy(packet, &ps, sizeof(PacketStructure));
	memcpy(packet + sizeof(PacketStructure), str.c_str(), str.length());
	CNetworkManager::getInstance().SendToServer(packet, ps.dataSize);
	delete[] packet;
}
