// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#ifndef STDAFX_H
#define STDAFX_H

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
// ���� �ð��� �޾ƿ��� �Լ�
void currentDateTime(char* curTime);

// ���� �ִ� ���� ������
#define MAX_SOCKET_BUFFER_SIZE 3472

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#endif