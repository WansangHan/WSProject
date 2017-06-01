#include "stdafx.h"
#include "Lobby.h"

std::unique_ptr<CLobby> CLobby::m_inst;
std::once_flag CLobby::m_once;

CLobby::CLobby()
{
}


CLobby::~CLobby()
{
}