// WSSockServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

