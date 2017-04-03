// WSSockServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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
	CIOCP* _iomodel = new CIOCP;
#else
	CEPOLL* _iomodel = new CEPOLL;
#endif
	if (_iomodel->InitServer())
		_iomodel->Update();
    return 0;
}

