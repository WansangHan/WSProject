// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// WSSockServer.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"

// TODO: �ʿ��� �߰� �����
// �� ������ �ƴ� STDAFX.H���� �����մϴ�.

// ���� �ð��� �޾ƿ��� �Լ�
void currentDateTime(char* curTime)
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[20];
#ifdef IOCP_SERVER
	localtime_s(&tstruct, &now);
#else
	tstruct = *localtime(&now);
#endif
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	memcpy(curTime, buf, 20);
	curTime[19] = '\0';
}

#ifndef IOCP_SERVER
// Linux�� ���� GetTickCount ����
unsigned int GetTickCount()
{
	timeval gettick;
	unsigned int tick;
	int ret;
	gettimeofday(&gettick, NULL);

	tick = gettick.tv_sec * 1000 + gettick.tv_usec / 1000;

	return tick;
}
#endif