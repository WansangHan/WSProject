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
			PacketInfo* info;
#ifdef IOCP_SERVER
			packet_queue_tcp.try_pop(info);
#else
			info = packet_queue_tcp.dequeue();
#endif
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
	if (it == tcp_function.end()) { CLogManager::getInstance().WriteLogMessage("ERROR", true, "tcp_function.end() : %d", packetType); return; }

	char* protoBuf = new char[MAX_SOCKET_BUFFER_SIZE];
	memcpy(protoBuf, info->data + sizeof(int) + sizeof(RecvPacketType), info->dataSize - sizeof(int) - sizeof(RecvPacketType));

	it->second(info->sock, protoBuf);
	delete protoBuf;
}

void CPacketManager::InitFunctionmap()
{
	tcp_function.insert(std::make_pair(RecvPacketType::RECV_SUM_INT, std::bind(&CLobby::SumData, CLobby::getInstance(), std::placeholders::_1, std::placeholders::_2)));
}

CPacketManager::~CPacketManager()
{
	delete th_tcp;
}

void CPacketManager::InitPacketManager()
{
	InitFunctionmap();
	th_tcp = new std::thread(&CPacketManager::APPLY_PACKET_TCP, this);
}

void CPacketManager::DEVIDE_PACKET_BUNDLE_TCP(CBaseSocket* sock, char * packet, int packetSize)
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
		info->SetVal(sock, data, Typesize);
#ifdef IOCP_SERVER
		packet_queue_tcp.push(info);
#else
		packet_queue_tcp.enqueue(info);
#endif
		curToken += Typesize;
	}
}

void CPacketManager::SendPacketToServer(CBaseSocket* sock, SendPacketType type, std::string str)
{
	PacketStructure ps;
	ps.packetType = type;
	ps.dataSize = str.length() + sizeof(PacketStructure);
	char* packet = new char[ps.dataSize];
	memcpy(packet, &ps, sizeof(PacketStructure));
	memcpy(packet + sizeof(PacketStructure), str.c_str(), str.length());
#ifdef IOCP_SERVER
	CIOCP::getInstance().PostSend(packet, ps.dataSize, sock, NULL, true);
#else
	CEPOLL::getInstance().SendToClient(packet, ps.dataSize, sock, NULL, true);
#endif
	delete[] packet;
}
