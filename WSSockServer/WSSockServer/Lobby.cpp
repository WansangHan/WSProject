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

void CLobby::SumData(std::shared_ptr<CBaseSocket> sock, char * message)
{

	Test::SumNumber result;

	result.ParseFromString(message);
	Test::SumNumber sendData;
	sendData.set_num(result.num());
	CPacketManager::getInstance().SendPacketToServer(sock, SendPacketType::SEND_SUM_INT, sendData.SerializeAsString(), nullptr, false);
}
