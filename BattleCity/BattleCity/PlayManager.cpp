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
}

void CPlayManager::EnterGame()
{
	BattleCity::EnterServer sendData;

	sendData.set__id(m_ownPlayer->GetID());
	sendData.set__name(m_ownPlayer->GetName());
	CPacketManager::getInstance().SendPacketToServer(SendPacketType::SD_ENTER_SERVER, sendData.SerializeAsString(), true, true);
}
