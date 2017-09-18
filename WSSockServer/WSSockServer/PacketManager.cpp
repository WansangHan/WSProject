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
		// Queue�� ���� Receive TCP ��Ŷ�� ó���ϴ� �Լ�
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
		// Queue�� ���� Receive UDP ��Ŷ�� ó���ϴ� �Լ�
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
	// ��Ŷ �и�
	memcpy(&packetType, info->data.get(), sizeof(RecvPacketType));
	// �Լ� ������ find
	auto it = map_function.find(packetType);
	if (it == map_function.end()) { CLogManager::getInstance().WriteLogMessage("ERROR", true, "map_function.end() : %d", packetType); return; }

	// ��Ŷ �� protobuffer ������ �߶� �Լ� ���ڷ� ����
	int charSize = info->dataSize - sizeof(int) - sizeof(RecvPacketType);
	std::shared_ptr<char> protoBuf = std::shared_ptr<char>(new char[charSize], std::default_delete<char[]>());
	memcpy(protoBuf.get(), info->data.get() + sizeof(int) + sizeof(RecvPacketType), charSize);

	it->second(info->sock, protoBuf.get(), charSize);
}

void CPacketManager::InitFunctionmap()
{
	// std::map�� ��Ŷ Ÿ�Կ� ���� �Լ������͸� ����
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
	// TCP, UDP ��Ŷ ó���ϴ� Thread ����
	th_tcp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_TCP, this));
	th_udp = std::unique_ptr<std::thread>(new std::thread(&CPacketManager::APPLY_PACKET_UDP, this));
}

void CPacketManager::DEVIDE_PACKET_BUNDLE(std::shared_ptr<CBaseSocket> sock, std::shared_ptr<char> packet, int packetSize, bool isTCP)
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
		memcpy(&Typesize, packet.get() + curToken + 4, sizeof(int));
		std::shared_ptr<char> data = std::shared_ptr<char>(new char[Typesize], std::default_delete<char[]>());
		// ��Ŷ �и�
		memcpy(data.get(), packet.get() + curToken, Typesize);
		std::shared_ptr<PacketInfo> info = std::make_shared<PacketInfo>();
		info->SetVal(sock, data, Typesize);
		// ��Ŷ ������ Queue�� push
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
	// ���ڷ� �Ѿ�� ��Ŷ Ÿ��, ��Ŷ�� ���� ��Ŷ ����� ������ �� ��Ŷ�� ���·� ����� Send �Լ��� ȣ����
	PacketStructure ps;
	ps.packetType = type;
	ps.dataSize = str.length() + sizeof(PacketStructure);
	std::shared_ptr<char> packet = std::shared_ptr<char>(new char[ps.dataSize], std::default_delete<char[]>());
	// ��Ŷ ����� 8����Ʈ ������ ��Ŷ�� Ÿ�԰� ����� memcpy
	memcpy(packet.get(), &ps, sizeof(PacketStructure));
	// ���� ������ protocolbuffer ������ �����͸� �̾����
	memcpy(packet.get() + sizeof(PacketStructure), str.c_str(), str.length());
#ifdef IOCP_SERVER
	CIOCP::getInstance().PostSend(packet.get(), ps.dataSize, sock, sockaddr, isTCP);
#else
	CEPOLL::getInstance().SendToClient(packet.get(), ps.dataSize, sock, sockaddr, isTCP);
#endif
}
