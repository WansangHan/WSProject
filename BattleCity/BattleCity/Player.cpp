#include "stdafx.h"
#include "Player.h"
#include "BattleCity.pb.h"
#include "PacketManager.h"

CPlayer::CPlayer()
{
}


CPlayer::~CPlayer()
{
}

// 사용자 키 입력을 받음
void CPlayer::KeyCheck() {
	ObjectDirection dir = ObjectDirection::IDLE;
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W') & 0x8000) {
		if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)			dir = ObjectDirection::UPLE;
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)		dir = ObjectDirection::UPRG;
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000)		dir = ObjectDirection::IDLE;
		else																			dir = ObjectDirection::UPUP;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
		if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)			dir = ObjectDirection::DWLE;
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)		dir = ObjectDirection::DWRG;
		else																			dir = ObjectDirection::DOWN;
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) {
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)			dir = ObjectDirection::IDLE;
		else																			dir = ObjectDirection::LEFT;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)			dir = ObjectDirection::RGHT;

	// 현재 방향값과 같다면 서버에 메시지를 보내지 않음
	if (dir == m_objectTransform->m_dir) return;

	// 좌표, 방향, 크기값을 담아 서버로 전송
	BattleCity::ObjectPosition* pos = new BattleCity::ObjectPosition;
	pos->set__id(m_ID);
	pos->set__vectorx(m_curPosition->m_vectorX);
	pos->set__vectory(m_curPosition->m_vectorY);
	BattleCity::ObjectTransform sendData;
	sendData.set_allocated__position(pos);
	sendData.set__dir((int)dir);
	sendData.set__scale(m_objectTransform->m_scale);
	sendData.set__speed(m_objectTransform->m_speed);

	CPacketManager::getInstance().SendPacketToServer(SD_POSITION_SCALE, sendData.SerializeAsString(), true, false);
}

// 서버에서 받은 키보드 방향에 맞게 이동
void CPlayer::PlayerMove()
{
	unsigned int DurTime = GetTickCount() - m_lastGetTickCount;
	float MoveTime = DurTime * 0.001f;

	float MoveSpeed = MoveTime* m_objectTransform->m_speed;

	switch (m_objectTransform->m_dir)
	{
	case ObjectDirection::IDLE:
		m_curPosition->m_vectorX = m_objectTransform->m_vectorX;
		m_curPosition->m_vectorY = m_objectTransform->m_vectorY;
		break;
	case ObjectDirection::UPUP:
		m_curPosition->m_vectorX = m_objectTransform->m_vectorX;
		m_curPosition->m_vectorY = m_objectTransform->m_vectorY - MoveSpeed;
		break;
	case ObjectDirection::LEFT:
		m_curPosition->m_vectorX = m_objectTransform->m_vectorX - MoveSpeed;
		m_curPosition->m_vectorY = m_objectTransform->m_vectorY;
		break;
	case ObjectDirection::RGHT:
		m_curPosition->m_vectorX = m_objectTransform->m_vectorX + MoveSpeed;
		m_curPosition->m_vectorY = m_objectTransform->m_vectorY;
		break;
	case ObjectDirection::DOWN:
		m_curPosition->m_vectorX = m_objectTransform->m_vectorX;
		m_curPosition->m_vectorY = m_objectTransform->m_vectorY + MoveSpeed;
		break;
	case ObjectDirection::UPLE:
		m_curPosition->m_vectorX = m_objectTransform->m_vectorX - (MoveSpeed * (1 / sqrt(2.0f)));
		m_curPosition->m_vectorY = m_objectTransform->m_vectorY - (MoveSpeed * (1 / sqrt(2.0f)));
		break;
	case ObjectDirection::UPRG:
		m_curPosition->m_vectorX = m_objectTransform->m_vectorX + (MoveSpeed * (1 / sqrt(2.0f)));
		m_curPosition->m_vectorY = m_objectTransform->m_vectorY - (MoveSpeed * (1 / sqrt(2.0f)));
		break;
	case ObjectDirection::DWLE:
		m_curPosition->m_vectorX = m_objectTransform->m_vectorX - (MoveSpeed * (1 / sqrt(2.0f)));
		m_curPosition->m_vectorY = m_objectTransform->m_vectorY + (MoveSpeed * (1 / sqrt(2.0f)));
		break;
	case ObjectDirection::DWRG:
		m_curPosition->m_vectorX = m_objectTransform->m_vectorX + (MoveSpeed * (1 / sqrt(2.0f)));
		m_curPosition->m_vectorY = m_objectTransform->m_vectorY + (MoveSpeed * (1 / sqrt(2.0f)));
		break;
	}
}