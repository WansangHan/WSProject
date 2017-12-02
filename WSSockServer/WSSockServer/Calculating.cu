#include "stdafx.h"
#include "Calculating.cuh"
#include <cuda.h>
#include <cufft.h>
#include <cublas_v2.h>
#include <cuda_runtime.h>
#include "LogManager.h"
#include "SyncServer.h"
#include "PacketManager.h"
#include "WSSockServer.pb.h"

std::unique_ptr<CCalculating> CCalculating::m_inst;
std::once_flag CCalculating::m_once;

CCalculating::CCalculating()
{
}


CCalculating::~CCalculating()
{
	isContinue = false;
	m_calculate_Thread->join();
}

// Object ���� ��ġ ����
__global__ void CalculateCurPosition(ObjectTransform lastMove, ObjectTransform* CurPos, unsigned int _lastTickCount, unsigned int _curTickCount)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	CurPos += tid;
	unsigned int DurTime = _curTickCount - _lastTickCount;
	float MoveTime = DurTime * 0.001f;

	float MoveSpeed = MoveTime* lastMove.m_speed;
	switch (lastMove.m_dir)
	{
	case ObjectDirection::IDLE:
		CurPos->m_vectorX = lastMove.m_vectorX;
		CurPos->m_vectorY = lastMove.m_vectorY;
		break;
	case ObjectDirection::UPUP:
		CurPos->m_vectorX = lastMove.m_vectorX;
		CurPos->m_vectorY = lastMove.m_vectorY - MoveSpeed;
		break;
	case ObjectDirection::LEFT:
		CurPos->m_vectorX = lastMove.m_vectorX - MoveSpeed;
		CurPos->m_vectorY = lastMove.m_vectorY;
		break;
	case ObjectDirection::RGHT:
		CurPos->m_vectorX = lastMove.m_vectorX + MoveSpeed;
		CurPos->m_vectorY = lastMove.m_vectorY;
		break;
	case ObjectDirection::DOWN:
		CurPos->m_vectorX = lastMove.m_vectorX;
		CurPos->m_vectorY = lastMove.m_vectorY + MoveSpeed;
		break;
	case ObjectDirection::UPLE:
		CurPos->m_vectorX = lastMove.m_vectorX - (MoveSpeed * (1 / sqrt(2.0f)));
		CurPos->m_vectorY = lastMove.m_vectorY - (MoveSpeed * (1 / sqrt(2.0f)));
		break;
	case ObjectDirection::UPRG:
		CurPos->m_vectorX = lastMove.m_vectorX + (MoveSpeed * (1 / sqrt(2.0f)));
		CurPos->m_vectorY = lastMove.m_vectorY - (MoveSpeed * (1 / sqrt(2.0f)));
		break;
	case ObjectDirection::DWLE:
		CurPos->m_vectorX = lastMove.m_vectorX - (MoveSpeed * (1 / sqrt(2.0f)));
		CurPos->m_vectorY = lastMove.m_vectorY + (MoveSpeed * (1 / sqrt(2.0f)));
		break;
	case ObjectDirection::DWRG:
		CurPos->m_vectorX = lastMove.m_vectorX + (MoveSpeed * (1 / sqrt(2.0f)));
		CurPos->m_vectorY = lastMove.m_vectorY + (MoveSpeed * (1 / sqrt(2.0f)));
		break;
	}
}

// �浹 ����
__global__ void CalculateCollision(ObjectTransform o1, ObjectTransform o2, bool* iscollision)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;

	float Distance = pow(o2.m_vectorX - o1.m_vectorX, 2) + pow(o2.m_vectorY - o1.m_vectorY, 2);

	iscollision[tid] = Distance < pow((o2.m_scale * 0.5) - (o1.m_scale * 0.5), 2);
}

// �÷��̾� ���̵�� ��Ī�Ǵ� Player Ŭ���� ������ ã�� �Լ�
std::shared_ptr<CPlayer> CCalculating::FindPlayerToID(int _pID)
{
	tbb::concurrent_hash_map<int, std::shared_ptr<CPlayer>>::accessor a;

	if (m_players.find(a, _pID))
	{
		return a->second;
	}
	else
	{
		CLogManager::getInstance().WriteLogMessage("WARN", true, "Return nullptr in FindPlayer()");
		return nullptr;
	}
}

// ��� ����ó���� ȣ���� �Լ�(������� ȣ��)
void CCalculating::CalculateAll()
{
	while (isContinue)
	{
		// �÷��̾��� ���� ��ǥ�� ���
		for (auto Pr : m_players)
		{
			ObjectTransform* temp;
			cudaMalloc((void**)&temp, sizeof(ObjectTransform) * 1);
			cudaMemcpy(temp, Pr.second->GetCurTransform().get(), sizeof(ObjectTransform) * 1, cudaMemcpyHostToDevice);
			CalculateCurPosition << <1, 1 >> > (*(Pr.second->GetTransform().get()), temp, Pr.second->GetLastGetTickCount(), GetTickCount());
			cudaMemcpy(Pr.second->GetCurTransform().get(), temp, sizeof(ObjectTransform) * 1, cudaMemcpyDeviceToHost);
			cudaFree(temp);
		}

		// �÷��̾ �浹 ����
		for (auto OutterPr : m_players)
		{
			for (auto InnerPr : m_players)
			{
				// �ڱ� �ڽŰ��� �浹 ������ �ƴ϶��
				if (OutterPr.first != InnerPr.first)
				{
					CPlayer smallPlayer;
					CPlayer bigerPlayer;

					// ���� ������Ʈ�� ū ������Ʈ ����
					if (InnerPr.second->GetTransform().get()->m_scale > OutterPr.second->GetTransform().get()->m_scale)
					{
						smallPlayer = *OutterPr.second.get();
						bigerPlayer = *InnerPr.second.get();
					}
					else
					{
						smallPlayer = *InnerPr.second.get();
						bigerPlayer = *OutterPr.second.get();
					}

					bool iscollision = false;
					bool* temp;
					cudaMalloc((void**)&temp, sizeof(bool) * 1);
					cudaMemcpy(temp, &iscollision, sizeof(bool), cudaMemcpyHostToDevice);
					CalculateCollision << <1, 1 >> > (*(smallPlayer.GetCurTransform().get()), *(bigerPlayer.GetCurTransform().get()), temp);
					cudaMemcpy(&iscollision, temp, sizeof(bool), cudaMemcpyDeviceToHost);
					cudaFree(temp);

					// �浹 ��
					if (iscollision)
					{
						WSSockServer::CollisionNotify SendData;
						SendData.set_smallplayerid(smallPlayer.GetID());
						SendData.set_bigerplayerid(bigerPlayer.GetID());
						// Sync �������� �浹�� �Ͼ ������Ʈ�� �˸�
						CSyncServer::getInstance().SendToSyncServer(SendPacketType::SD_COLLISION_NOTIFY, SendData.SerializeAsString(), true);
					}
				}
			}
		}
	}
}

void CCalculating::InitCalculating()
{
	srand((unsigned int)time(NULL));

	isContinue = true;

	m_calculate_Thread = std::unique_ptr<std::thread>(new std::thread([&]() { this->CalculateAll(); }));
}

// ������Ʈ���� ���� ��ġ�� ���ؼ� ����ȭ ������ �����ش�
void CCalculating::SetStartingPosition(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	float vectorX = rand() % 601;
	float vectorY = rand() % 601;
	float scale = rand() % 50;
	float speed = 20;

	WSSockServer::ObjectPosition* pos = new WSSockServer::ObjectPosition;
	pos->set__id(RecvData._id());
	pos->set__vectorx(vectorX);
	pos->set__vectory(vectorY);
	WSSockServer::ObjectTransform SendData;
	SendData.set_allocated__position(pos);
	SendData.set__scale(scale);
	SendData.set__speed(speed);
	SendData.set__dir((int)ObjectDirection::IDLE);
	CSyncServer::getInstance().SendToSyncServer(SendPacketType::SD_AI_STARTING, SendData.SerializeAsString(), true);
}

// �÷��̾� ���� �� ���� �������� Ŭ���̾�Ʈ ������ ���� �� ���� ��ǥ ����
void CCalculating::EnterPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::PlayerInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	float vectorX = rand() % 601;
	float vectorY = rand() % 601;
	float scale = rand() % 50;
	float speed = 20;

	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());

	ObjectTransform playerTransform(vectorX, vectorY, scale, speed, ObjectDirection::IDLE);
	player->SetTransform(playerTransform);
	player->SetCurTransform(playerTransform);
	player->SetLastGetTickCount(GetTickCount());

	m_players.insert(std::map<int, std::shared_ptr<CPlayer>>::value_type(player->GetID(), player));

	WSSockServer::ObjectPosition* pos = new WSSockServer::ObjectPosition;
	pos->set__id(RecvData._id());
	pos->set__vectorx(vectorX);
	pos->set__vectory(vectorY);
	WSSockServer::ObjectTransform SendData;
	SendData.set_allocated__position(pos);
	SendData.set__scale(scale);
	SendData.set__speed(speed);
	CSyncServer::getInstance().SendToSyncServer(SendPacketType::SD_ENTER_PLAYER_CALC, SendData.SerializeAsString(), true);
}

// �÷��̾� ������ ���� ����
void CCalculating::ApplyPlayerSocket(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());

	player->SetSocket(_sock);

	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_CALC_TCP, "", nullptr, true);
}

// �÷��̾� ������ UDP ��巹�� ����
void CCalculating::ApplyPlayerUDP(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char * _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	player->SetAddr(_addr);

	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_CALC_UDP, "", nullptr, true);
}

// �÷��̾ ������ ��, �ش� ���̵� �´� �÷��̾� ����
void CCalculating::ExitPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	m_players.erase(RecvData._id());
}

// Ŭ���̾�Ʈ�� ������ �̵� �̺�Ʈ ������ ����
void CCalculating::ApplyPlayerTrasform(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char * _data, int _size)
{
	WSSockServer::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._position()._id());
	player->SetLastGetTickCount(GetTickCount());
	ObjectTransform playerTransform(RecvData._position()._vectorx(), RecvData._position()._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	player->SetTransform(playerTransform);
	player->SetCurTransform(playerTransform);
}

// ������ �÷��̾��� ũ�� ����
void CCalculating::IncreaseScale(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char * _data, int _size)
{
	WSSockServer::IncreaseScale RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());

	std::shared_ptr<ObjectTransform> temp = player->GetTransform();
	temp->m_scale = RecvData._increase();
	player->SetTransform(*temp.get());
}
