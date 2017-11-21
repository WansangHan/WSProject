#include "stdafx.h"
#include "Player.h"


CPlayer::CPlayer()
{
	m_objectTransform = std::make_shared<ObjectTransform>();
#ifdef IOCP_SERVER
	m_curPosition= std::make_shared<ObjectTransform>();
#endif
}


CPlayer::~CPlayer()
{
}