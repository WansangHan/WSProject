#include "stdafx.h"
#include "InGame.h"

std::unique_ptr<CInGame> CInGame::m_inst;
std::once_flag CInGame::m_once;

CInGame::CInGame()
{
}


CInGame::~CInGame()
{
}

void CInGame::EnterPlayer(std::shared_ptr<CBaseSocket> _sock, char* _data)
{
	WSSockServer::EnterServer RecvData;

	RecvData.ParseFromString(_data);

	std::cout << RecvData._id() << std::endl;
	std::cout << RecvData._name() << std::endl;
}
