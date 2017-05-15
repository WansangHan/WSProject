#include "stdafx.h"
#include "GameManager.h"

std::unique_ptr<CGameManager> CGameManager::m_inst;
std::once_flag CGameManager::m_once;

CGameManager::CGameManager()
{
}


CGameManager::~CGameManager()
{
}

bool CGameManager::InitGameManager()
{
	CLogManager::getInstance().InitLogManager();
	CNetworkManager::getInstance().InitNetworkManager();
	CPacketManager::getInstance().InitPacketManager();

	for (int i = 1; i <= 50000; i++)
	{
		Test::SumNumber numb;
		numb.set_num(i);

		CPacketManager::getInstance().SendPacketToServer(SendPacketType::SEND_SUM_INT, numb.SerializeAsString());
	}

	return false;
}

void CGameManager::SumData(char * message)
{
	Test::SumNumber result;

	result.ParseFromString(message);
	
	static int sum = 0;
	sum += result.num();
	if(result.num() == 50000)
		CLogManager::getInstance().WriteLogMessage("INFO", true, "sum : %d", sum);

}


