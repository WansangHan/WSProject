// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

//#define IOCP_SERVER

#ifdef IOCP_SERVER
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include "targetver.h"

#include <tchar.h>
#else
#endif

#include <stdio.h>
#include <time.h>

#ifdef IOCP_SERVER
#define SERVER_CATE "IOCP"
#else
#define SERVER_CATE "EPOL"
#endif

void currentDateTime(char* curTime);

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
