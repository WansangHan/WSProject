// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
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

// ���� �ð��� �޾ƿ��� �Լ�
void currentDateTime(char* curTime);

#ifndef IOCP_SERVER
// Linux������ ����ϴ� GetTickCount�Լ�
unsigned int GetTickCount();
#endif

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#endif