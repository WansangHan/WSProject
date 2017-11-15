#include "stdafx.h"
#include "Player.h"


CPlayer::CPlayer()
{
}


CPlayer::~CPlayer()
{
}


#ifdef IOCP_SERVER
void CPlayer::CalculateCurrentPosition()
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
		m_curPosition->m_vectorY = m_objectTransform->m_vectorY + MoveTime;
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
#endif