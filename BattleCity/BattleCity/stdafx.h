// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#ifndef STDAFX_H
#define STDAFX_H

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
// 현재 시간을 받아오는 함수
void currentDateTime(char* curTime);

// 소켓 최대 버퍼 사이즈
#define MAX_SOCKET_BUFFER_SIZE 3472

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#endif