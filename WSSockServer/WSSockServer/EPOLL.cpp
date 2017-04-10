#include "stdafx.h"
#include "EPOLL.h"

std::unique_ptr<CEPOLL> CEPOLL::m_inst;
std::once_flag CEPOLL::m_once;

CEPOLL::CEPOLL()
{
}


CEPOLL::~CEPOLL()
{
}

bool CEPOLL::InitServer()
{
	CLogManager::getInstance().InitLogManager();
	CLogManager::getInstance().WriteLogMessage("ERROR", true, "WSAStartup Error%d", 10);
	return true;
}

void CEPOLL::Update()
{
	while (1) { }
}

void CEPOLL::CloseServer()
{

}