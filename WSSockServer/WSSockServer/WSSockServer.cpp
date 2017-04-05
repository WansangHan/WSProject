// WSSockServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#ifdef IOCP_SERVER
#include "IOCP.h"
#else
#include "EPOLL.h"
#endif

int main()
{
#ifdef IOCP_SERVER
	if (CIOCP::getInstance().InitServer())
		CIOCP::getInstance().Update();
#else
	if (CEPOLL::getInstance().InitServer())
		CEPOLL::getInstance().Update();
#endif
	
    return 0;
}

