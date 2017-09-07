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
	int left = m_vectorX - m_scale / 2;
	int top = m_vectorY - m_scale / 2;
	int right = m_vectorX + m_scale / 2;
	int bottom = m_vectorY + m_scale / 2;
	Ellipse(_hdc, left, top, right, bottom);
}
