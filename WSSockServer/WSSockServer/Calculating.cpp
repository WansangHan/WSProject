#include "stdafx.h"
#include "Calculating.h"

std::unique_ptr<CCalculating> CCalculating::m_inst;
std::once_flag CCalculating::m_once;

CCalculating::CCalculating()
{
}


CCalculating::~CCalculating()
{
}

void CCalculating::InitCalculating()
{
	srand((unsigned int)time(NULL));
}

// ������Ʈ���� ���� ��ġ�� ���ؼ� IOCP�� �����ش�
void CCalculating::SetStartingPosition(std::shared_ptr<CBaseSocket> _sock, char * _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	float vectorX = rand() % 601;
	float vectorY = rand() % 601;
	float scale = rand() % 50;

	WSSockServer::ObjectTransform SendData;
	SendData.set__id(RecvData._id());
	SendData.set__vectorx(vectorX);
	SendData.set__vectory(vectorY);
	SendData.set__scale(scale);
	SendData.set__dir((int)ObjectDirection::IDLE);
	CSyncServer::getInstance().SendToSyncServer(SendPacketType::SD_AI_STARTING, SendData.SerializeAsString(), true);
}

// �÷��̾� ���� �� EPOLL �������� Ŭ���̾�Ʈ ������ ����
void CCalculating::EnterPlayer(std::shared_ptr<CBaseSocket> _sock, char * _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());

	m_players.insert(std::map<int, std::shared_ptr<CPlayer>>::value_type(player->GetID(), player));

	CSyncServer::getInstance().SendToSyncServer(SendPacketType::SD_ENTER_PLAYER_EPOLL, RecvData.SerializeAsString(), true);
}
