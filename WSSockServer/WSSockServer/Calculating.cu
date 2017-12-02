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

// Object 현재 위치 연산
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

// 충돌 연산
__global__ void CalculateCollision(ObjectTransform o1, ObjectTransform o2, bool* iscollision)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;

	float Distance = pow(o2.m_vectorX - o1.m_vectorX, 2) + pow(o2.m_vectorY - o1.m_vectorY, 2);

	iscollision[tid] = Distance < pow((o2.m_scale * 0.5) - (o1.m_scale * 0.5), 2);
}

// 플레이어 아이디와 매칭되는 Player 클래스 변수를 찾는 함수
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

// 모든 연산처리를 호출할 함수(쓰레드로 호출)
void CCalculating::CalculateAll()
{
	while (isContinue)
	{
		// 플레이어의 현재 좌표를 계산
		for (auto Pr : m_players)
		{
			ObjectTransform* temp;
			cudaMalloc((void**)&temp, sizeof(ObjectTransform) * 1);
			cudaMemcpy(temp, Pr.second->GetCurTransform().get(), sizeof(ObjectTransform) * 1, cudaMemcpyHostToDevice);
			CalculateCurPosition << <1, 1 >> > (*(Pr.second->GetTransform().get()), temp, Pr.second->GetLastGetTickCount(), GetTickCount());
			cudaMemcpy(Pr.second->GetCurTransform().get(), temp, sizeof(ObjectTransform) * 1, cudaMemcpyDeviceToHost);
			cudaFree(temp);
		}

		// 플레이어간 충돌 연산
		for (auto OutterPr : m_players)
		{
			for (auto InnerPr : m_players)
			{
				// 자기 자신과의 충돌 연산이 아니라면
				if (OutterPr.first != InnerPr.first)
				{
					CPlayer smallPlayer;
					CPlayer bigerPlayer;

					// 작은 오브젝트와 큰 오브젝트 구별
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

					// 충돌 시
					if (iscollision)
					{
						WSSockServer::CollisionNotify SendData;
						SendData.set_smallplayerid(smallPlayer.GetID());
						SendData.set_bigerplayerid(bigerPlayer.GetID());
						// Sync 서버에게 충돌이 일어난 오브젝트를 알림
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

// 오브젝트들의 시작 위치를 정해서 동기화 서버로 보내준다
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

// 플레이어 접속 시 연산 서버에도 클라이언트 정보를 저장 및 시작 좌표 리턴
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

// 플레이어 변수에 소켓 적용
void CCalculating::ApplyPlayerSocket(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());

	player->SetSocket(_sock);

	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_CALC_TCP, "", nullptr, true);
}

// 플레이어 변수에 UDP 어드레스 적용
void CCalculating::ApplyPlayerUDP(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char * _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	player->SetAddr(_addr);

	CPacketManager::getInstance().SendPacketToServer(player->GetSocket(), SendPacketType::SD_SUCCESS_CALC_UDP, "", nullptr, true);
}

// 플레이어가 나갔을 때, 해당 아이디에 맞는 플레이어 삭제
void CCalculating::ExitPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size)
{
	WSSockServer::ObjectInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	m_players.erase(RecvData._id());
}

// 클라이언트의 마지막 이동 이벤트 정보를 저장
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

// 증가한 플레이어의 크기 적용
void CCalculating::IncreaseScale(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char * _data, int _size)
{
	WSSockServer::IncreaseScale RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());

	std::shared_ptr<ObjectTransform> temp = player->GetTransform();
	temp->m_scale = RecvData._increase();
	player->SetTransform(*temp.get());
}
