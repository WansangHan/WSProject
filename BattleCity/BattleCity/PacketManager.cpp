#include "stdafx.h"
#include "PacketManager.h"

std::unique_ptr<CPacketManager> CPacketManager::m_inst;
std::once_flag CPacketManager::m_once;

CPacketManager::CPacketManager()
{
}

void CPacketManager::APPLY_PACKET()
{
	// Queue�� ���� Receive TCP ��Ŷ�� ó��
	if (!packet_queue_tcp.empty())
	{
		std::shared_ptr<PacketInfo> info;
		packet_queue_tcp.try_pop(info);
		DEVIDE_PACKET_TYPE(info);
	}
	// Queue�� ���� Receive UDP ��Ŷ�� ó��
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
	// ��Ŷ �и�
	memcpy(&packetType, info->data.get(), sizeof(RecvPacketType));
	// �Լ� ������ find
	auto it = map_function.find(packetType);
	if (it == map_function.end()) { CLogManager::getInstance().WriteLogMessage("ERROR", true, "map_function.end() : %d", packetType); return; }

	// ��Ŷ �� protobuffer ������ �߶� �Լ� ���ڷ� ����
	int charSize = info->dataSize - sizeof(int) - sizeof(RecvPacketType);
	std::shared_ptr<char> protoBuf = std::shared_ptr<char>(new char[charSize], std::default_delete<char[]>());
	memcpy(protoBuf.get(), info->data.get() + sizeof(int) + sizeof(RecvPacketType), charSize);
	it->second(protoBuf.get(), charSize);
}

void CPacketManager::InitFunctionmap()
{
	// std::map�� ��Ŷ Ÿ�Կ� ���� �Լ������͸� �����ϴ� �κ�
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
	// Thread�� ������ ���Ḧ ���� Join
}

void CPacketManager::DEVIDE_PACKET_BUNDLE(char * packet, int packetSize, bool isTCP)
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
		if(isTCP) packet_queue_tcp.push(info);
		else packet_queue_udp.push(info);
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
