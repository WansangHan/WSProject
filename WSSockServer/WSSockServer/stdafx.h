// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <stdio.h>
#include <time.h>

#define SERVER_CATE "IOCP"

void currentDateTime(char* curTime);

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
