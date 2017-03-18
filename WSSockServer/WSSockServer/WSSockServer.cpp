// WSSockServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "IOCP.h"

int main()
{
	CIOCP* _iocp = new CIOCP;
	if (_iocp->InitServer())
		_iocp->Update();
    return 0;
}

