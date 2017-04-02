// WSSockServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#ifdef IOCP_SERVER
#include "IOCP.h"
#else
#endif

int main()
{
#ifdef IOCP_SERVER
	CIOCP* _iomodel = new CIOCP;
	if (_iomodel->InitServer())
		_iomodel->Update();
#else
	printf("Epoll_Server!!\n");
#endif
    return 0;
}

