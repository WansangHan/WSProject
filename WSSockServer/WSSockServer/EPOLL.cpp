#include "stdafx.h"
#include "EPOLL.h"


CEPOLL::CEPOLL()
{
}


CEPOLL::~CEPOLL()
{
}

bool CEPOLL::InitServer()
{
	CLogManager::getInstance().InitLogManager();

	return true;
}

void CEPOLL::Update()
{
	while (1) { }
}

void CEPOLL::CloseServer()
{

}