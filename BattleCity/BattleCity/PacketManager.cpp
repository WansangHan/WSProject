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
		// Queue�� ���� Receive TCP ��Ŷ�� ó���ϴ� �Լ� (���� ���� �����忡�� �����ų ����)
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
		// Queue�� ���� Receive UDP ��Ŷ�� ó���ϴ� �Լ� (���� ���� �����忡�� �����ų ����)
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
	// ��Ŷ �и�
	memcpy(&packetType, info->data.get(), sizeof(RecvPacketType));
	// �Լ� ������ find
	auto it = map_function.find(packetType);
	if (it == map_function.end()) { CLogManager::getInstance().WriteLogMessage("ERROR", true, "map_function.end() : ", packetType); return; }

	// ��Ŷ �� protobuffer ������ �߶� �Լ� ���ڷ� ����
	std::shared_ptr<char> protoBuf = std::shared_ptr<char>(new char[info->dataSize - sizeof(int) - sizeof(RecvPacketType)], std::default_delete<char[]>());
	memcpy(protoBuf.get(), info->data.get() + sizeof(int) + sizeof(RecvPacketType), info->dataSize - sizeof(int) - sizeof(RecvPacketType));

	it->second(protoBuf.get());
}

void CPacketManager::InitFunctionmap()
{
	// std::map�� ��Ŷ Ÿ�Կ� ���� �Լ������͸� �����ϴ� �κ�
}

CPacketManager::~CPacketManager()
{
}

void CPacketManager::InitPacketManager()
{
	InitFunctionmap();
	isContinue = true;
	// TCP, UDP ��Ŷ ó���ϴ� Thread ����
	th_tcp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_TCP, this));
	th_udp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_UDP, this));
}

void CPacketManager::ExitPacketManager()
{
	isContinue = false;
	// Thread�� ������ ���Ḧ ���� Join
	th_tcp->join();
	th_udp->join();
}

void CPacketManager::DEVIDE_PACKET_BUNDLE(char * packet, int packetSize)
{
	// ��Ŷ�� ���ļ� ���� �� �������ִ� �Լ�
	// ��Ŷ Ÿ�� 4Byte / ��Ŷ ������ 4Byte / ���� protobuf ����
	// ���� ���� ��Ŷ ������ �Ǿ��ְ�, ��Ŷ ������ 4����Ʈ�� �޾ƿ� �и��Ѵ�.
	int curToken = 0;
	int Typesize = 0;
	while (packetSize > curToken)
	{
		Typesize = 0;
		// ������ Ȯ��
		memcpy(&Typesize, packet + curToken + 4, sizeof(int));
		std::shared_ptr<char> data = std::shared_ptr<char>(new char[MAX_SOCKET_BUFFER_SIZE], std::default_delete<char[]>());
		// ��Ŷ �и�
		memcpy(data.get(), packet + curToken, Typesize);
		std::shared_ptr<PacketInfo> info = std::make_shared<PacketInfo>();
		info->SetVal(data, Typesize);
		// ��Ŷ ������ Queue�� push
		packet_queue_tcp.push(info);
		curToken += Typesize;
	}
}

void CPacketManager::SendPacketToServer(SendPacketType type, std::string str, bool isTCP, bool isIOCP)
{
	// ���ڷ� �Ѿ�� ��Ŷ Ÿ��, ��Ŷ�� ���� ��Ŷ ����� ������ �� ��Ŷ�� ���·� ����� SendToServer �Լ��� ȣ����
	PacketStructure ps;
	ps.packetType = type;
	ps.dataSize = str.length() + sizeof(PacketStructure);
	std::shared_ptr<char> packet = std::shared_ptr<char>(new char[ps.dataSize], std::default_delete<char[]>());
	// ��Ŷ ����� 8����Ʈ ������ ��Ŷ�� Ÿ�԰� ����� memcpy
	memcpy(packet.get(), &ps, sizeof(PacketStructure));
	// ���� ������ protocolbuffer ������ �����͸� �̾����
	memcpy(packet.get() + sizeof(PacketStructure), str.c_str(), str.length());
	CNetworkManager::getInstance().SendToServer(packet.get(), ps.dataSize, isTCP, isIOCP);
}
