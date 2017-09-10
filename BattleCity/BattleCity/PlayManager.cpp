#include "stdafx.h"
#include "PlayManager.h"


CPlayManager::CPlayManager()
{
}


CPlayManager::~CPlayManager()
{
	delete m_ownPlayer;
}

void CPlayManager::InitPlayerManager()
{
	m_ownPlayer = new CPlayer;
}

void CPlayManager::PaintPlay(HWND _hwnd, HDC _hdc)
{
	m_ownPlayer->PaintPlayer(_hwnd, _hdc);
}

// 게임 플레이 시 매 프레임 업데이트 되는 부분
void CPlayManager::UpdatePlay()
{
	m_ownPlayer->PlayerMove();
}

void CPlayManager::EnterGame()
{
	BattleCity::EnterServer sendData;

	sendData.set__id(m_ownPlayer->GetID());
	sendData.set__name(m_ownPlayer->GetName());

	// 현재 클라이언트의 정보를 받아 서버로 전달
	CPacketManager::getInstance().SendPacketToServer(SendPacketType::SD_ENTER_SERVER, sendData.SerializeAsString(), true, true);
}

// 서버로부터 받은 좌표, 크기, 방향 정보를 플레이어에 저장
void CPlayManager::SetPositionScale(char * _data, int _size)
{
	BattleCity::SetPositionScale RecvData;
	RecvData.ParseFromArray(_data, _size);

	if (m_ownPlayer->GetID() == RecvData._id())
	{
		m_ownPlayer->SetXY(RecvData._vectorx(), RecvData._vectory());
		m_ownPlayer->SetScale(RecvData._scale());
		m_ownPlayer->SetDir(RecvData._dir());
	}
}

// 키보드 이벤트 시
void CPlayManager::CheckKey()
{
	m_ownPlayer->KeyCheck();
}
