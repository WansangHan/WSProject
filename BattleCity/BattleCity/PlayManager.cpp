#include "stdafx.h"
#include "PlayManager.h"


CPlayManager::CPlayManager()
{
}


CPlayManager::~CPlayManager()
{
}

// 플레이어 아이디와 매칭되는 Player 클래스 변수를 찾는 함수
std::shared_ptr<CPlayer> CPlayManager::FindPlayerToID(int _pID)
{
	for (auto Pr : m_otherPlayer)
	{
		if (Pr->GetID() == _pID)
			return Pr;
	}
	CLogManager::getInstance().WriteLogMessage("WARN", true, "Return nullptr in FindPlayer()");
	return nullptr;
}

void CPlayManager::InitPlayerManager()
{
}

void CPlayManager::PaintPlay(HWND _hwnd, HDC _hdc)
{
	m_ownPlayer->PaintPlayer(_hwnd, _hdc);
	for (auto Pr : m_otherPlayer)
	{
		Pr->PaintPlayer(_hwnd, _hdc);
	}
	for (auto Ao : m_AIObjects)
	{
		Ao->PaintAIObject(_hwnd, _hdc);
	}
}

// 게임 플레이 시 매 프레임 업데이트 되는 부분
void CPlayManager::UpdatePlay()
{
	m_ownPlayer->PlayerMove();
	for (auto Pr : m_otherPlayer)
	{
		Pr->PlayerMove();
	}
}

void CPlayManager::EnterGame(int _id, std::string _name)
{
	m_ownPlayer = std::make_shared<CPlayer>();
	m_ownPlayer->SetID(_id);
	m_ownPlayer->SetName(_name);

	BattleCity::PlayerInformation sendData;
	sendData.set__id(_id);
	sendData.set__name(_name);

	// 현재 클라이언트의 정보를 받아 서버로 전달
	CPacketManager::getInstance().SendPacketToServer(SendPacketType::SD_ENTER_IOCP_SERVER, sendData.SerializeAsString(), true, true);
}

// 자기 자신이 아닌 상대 플레이어에 대한 정보를 list에 저장한다
void CPlayManager::EnterPlayer(char * _data, int _size)
{
	BattleCity::PlayerInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<CPlayer> player = std::make_shared<CPlayer>();
	player->SetID(RecvData._id());
	player->SetName(RecvData._name());
	
	// 상대 플레이어를 리스트에 추가
	m_otherPlayer.push_back(player);
}

// 다른 플레이어들이 나갔을 때 처리
void CPlayManager::ExitPlayer(char * _data, int _size)
{
	BattleCity::PlayerInformation RecvData;
	RecvData.ParseFromArray(_data, _size);

	// 나간 플레이어를 찾아 삭제
	std::shared_ptr<CPlayer> player = FindPlayerToID(RecvData._id());
	m_otherPlayer.remove(player);
}

// 서버로부터 받은 좌표, 크기, 방향 정보를 플레이어에 저장
void CPlayManager::SetPlayerPositionScale(char * _data, int _size)
{
	BattleCity::ObjectTransform RecvData;
	RecvData.ParseFromArray(_data, _size);

	std::shared_ptr<ObjectTransform> playerTransform = std::make_shared<ObjectTransform>(RecvData._position()._vectorx(), RecvData._position()._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	// 자기 자신에 대한 위치 정보라면
	if (m_ownPlayer->GetID() == RecvData._position()._id())
	{
		m_ownPlayer->SetTransform(playerTransform);
		m_ownPlayer->SetLastGetTickCount(GetTickCount());
	}
	else
	{
		// 자기 자신이 아니라면 플레이어를 찾아 Trasform 정보를 적용한다
		for (auto Pr : m_otherPlayer)
		{
			if (Pr->GetID() == RecvData._position()._id())
			{
				Pr->SetTransform(playerTransform);
				Pr->SetLastGetTickCount(GetTickCount());
			}
		}
	}
}

// 서버로부터 받은 Transform 정보를 토대로 AIObject를 만들어 List에 저장
void CPlayManager::SetAIObjectPositionScale(char * _data, int _size)
{
	BattleCity::ObjectTransform RecvData;

	RecvData.ParseFromArray(_data, _size);
	std::shared_ptr<ObjectTransform> aiObjectTransform = std::make_shared<ObjectTransform>(RecvData._position()._vectorx(), RecvData._position()._vectory(), RecvData._scale(), RecvData._speed(), (ObjectDirection)RecvData._dir());
	
	std::shared_ptr<CAIObject> aiObject = std::make_shared<CAIObject>();
	aiObject->SetID(RecvData._position()._id());
	aiObject->SetTransform(aiObjectTransform);

	m_AIObjects.push_back(aiObject);
}

// 키보드 이벤트 시
void CPlayManager::CheckKey()
{
	m_ownPlayer->KeyCheck();
}
