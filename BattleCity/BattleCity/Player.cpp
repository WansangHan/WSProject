#include "stdafx.h"
#include "Player.h"


CPlayer::CPlayer()
{
}


CPlayer::~CPlayer()
{
}

// 좌표, 크기값을 기반으로 플레이어를 그림
void CPlayer::PaintPlayer(HWND _hwnd, HDC _hdc)
{
	int left = m_playerTransform.m_vectorX - m_playerTransform.m_scale / 2;
	int top = m_playerTransform.m_vectorY - m_playerTransform.m_scale / 2;
	int right = m_playerTransform.m_vectorX + m_playerTransform.m_scale / 2;
	int bottom = m_playerTransform.m_vectorY + m_playerTransform.m_scale / 2;
	Ellipse(_hdc, left, top, right, bottom);
}

// 사용자 키 입력을 받음
void CPlayer::KeyCheck() {
	PlayerDirection dir = IDLE;
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W') & 0x8000) {
		if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)			dir = UPLE;
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)		dir = UPRG;
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000)		dir = IDLE;
		else																			dir = UPUP;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
		if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)			dir = DWLE;
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)		dir = DWRG;
		else																			dir = DOWN;
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) {
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)			dir = IDLE;
		else																			dir = LEFT;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)			dir = RGHT;

	// 현재 방향값과 같다면 서버에 메시지를 보내지 않음
	if (dir == m_playerTransform.m_dir) return;

	// 좌표, 방향, 크기값을 담아 서버로 전송
	BattleCity::PlayerTransform sendData;

	sendData.set__id(m_ID);
	sendData.set__vectorx(m_playerTransform.m_vectorX);
	sendData.set__vectory(m_playerTransform.m_vectorY);
	sendData.set__dir(dir);
	sendData.set__scale(m_playerTransform.m_scale);

	CPacketManager::getInstance().SendPacketToServer(SD_POSITION_SCALE, sendData.SerializeAsString(), true, true);
	
}

// 서버에서 받은 키보드 방향에 맞게 이동
void CPlayer::PlayerMove()
{
	switch (m_playerTransform.m_dir)
	{
	case UPUP:
		m_playerTransform.m_vectorY -= 1;
		break;
	case LEFT:
		m_playerTransform.m_vectorX -= 1;
		break;
	case RGHT:
		m_playerTransform.m_vectorX += 1;
		break;
	case DOWN:
		m_playerTransform.m_vectorY += 1;
		break;
	case UPLE:
		m_playerTransform.m_vectorX -= 1 / sqrt(2.0f);
		m_playerTransform.m_vectorY -= 1 / sqrt(2.0f);
		break;
	case UPRG:
		m_playerTransform.m_vectorX += 1 / sqrt(2.0f);
		m_playerTransform.m_vectorY -= 1 / sqrt(2.0f);
		break;
	case DWLE:
		m_playerTransform.m_vectorX -= 1 / sqrt(2.0f);
		m_playerTransform.m_vectorY += 1 / sqrt(2.0f);
		break;
	case DWRG:
		m_playerTransform.m_vectorX += 1 / sqrt(2.0f);
		m_playerTransform.m_vectorY += 1 / sqrt(2.0f);
		break;
	}
}
