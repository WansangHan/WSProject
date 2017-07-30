// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// WSSockServer.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"
#include <iostream>

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.

// 현재 시간을 받아오는 함수
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