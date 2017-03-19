#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <iostream>

#include "LogManager.h"

class CIOCP
{
	WSADATA m_wsaData;

public:
	CIOCP();
	~CIOCP();

	bool InitServer();
	void Update();
	void CloseServer();
};

