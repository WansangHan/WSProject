// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// WSSockServer.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"
#include <iostream>

// TODO: �ʿ��� �߰� �����
// �� ������ �ƴ� STDAFX.H���� �����մϴ�.

void currentDateTime(char* curTime)
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[20];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	memcpy(curTime, buf, 20);
	curTime[19] = '\0';
	std::cout << curTime << std::endl;
}