#pragma once


#include "SqlManager.h"
#include <WinSock2.h>
#include <windows.h>

class CIOCP
{
	WSADATA m_wsaData;

	CSqlManager* m_sqlmanager;
public:
	CIOCP();
	~CIOCP();

	bool InitServer();
	void Update();
	void CloseServer();
};

