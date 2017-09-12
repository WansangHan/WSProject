#include "stdafx.h"
#include "Player.h"


CPlayer::CPlayer()
{
}


CPlayer::~CPlayer()
{
}

// ��ǥ, ũ�Ⱚ�� ������� �÷��̾ �׸�
void CPlayer::PaintPlayer(HWND _hwnd, HDC _hdc)
{
	int left = m_playerTransform.m_vectorX - m_playerTransform.m_scale / 2;
	int top = m_playerTransform.m_vectorY - m_playerTransform.m_scale / 2;
	int right = m_playerTransform.m_vectorX + m_playerTransform.m_scale / 2;
	int bottom = m_playerTransform.m_vectorY + m_playerTransform.m_scale / 2;
	Ellipse(_hdc, left, top, right, bottom);
}

// ����� Ű �Է��� ����
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

	// ���� ���Ⱚ�� ���ٸ� ������ �޽����� ������ ����
	if (dir == m_playerTransform.m_dir) return;

	// ��ǥ, ����, ũ�Ⱚ�� ��� ������ ����
	BattleCity::PlayerTransform sendData;

	sendData.set__id(m_ID);
	sendData.set__vectorx(m_playerTransform.m_vectorX);
	sendData.set__vectory(m_playerTransform.m_vectorY);
	sendData.set__dir(dir);
	sendData.set__scale(m_playerTransform.m_scale);

	CPacketManager::getInstance().SendPacketToServer(SD_POSITION_SCALE, sendData.SerializeAsString(), true, true);
	
}

// �������� ���� Ű���� ���⿡ �°� �̵�
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
