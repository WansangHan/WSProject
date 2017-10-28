// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#ifndef STDAFX_H
#define STDAFX_H

#define IOCP_SERVER

#ifdef IOCP_SERVER
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include "targetver.h"

#include <tchar.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include <memory.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef IOCP_SERVER
#define SERVER_CATE "IOCP"
#else
#define SERVER_CATE "EPOL"
#endif
#define MAX_SOCKET_BUFFER_SIZE 3472
#define WORKERTHREAD_NUM 8

// 현재 시간을 받아오는 함수
void currentDateTime(char* curTime);

#ifndef IOCP_SERVER
// Linux에서만 사용하는 GetTickCount함수
unsigned int GetTickCount();
#endif

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#endif