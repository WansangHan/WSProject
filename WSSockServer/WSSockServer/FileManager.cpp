#include "stdafx.h"
#include "FileManager.h"
#include <iostream>

CFileManager::CFileManager()
{
}


CFileManager::~CFileManager()
{
}

// 파일에 로그를 쓰는 함수
bool CFileManager::WriteFileLog(const char * _message, char * _level)
{
	std::shared_ptr<char> curDate = std::shared_ptr<char>(new char[20], std::default_delete<char[]>());
	currentDateTime(curDate.get());
	std::shared_ptr<char> fileDir = std::shared_ptr<char>(new char[20], std::default_delete<char[]>());
	std::shared_ptr<char> fileName = std::shared_ptr<char>(new char[11], std::default_delete<char[]>());
	// 날짜를 파일 이름으로
	memcpy(fileName.get(), curDate.get(), 10);
	fileName.get()[10] = '\0';
	// 디렉토리, 파일 확장자 적용
#ifdef IOCP_SERVER
	sprintf_s(fileDir.get(), 20,
		"logs/%s.txt",
		fileName.get());
#else
	sprintf(fileDir.get(),
		"logs/%s.txt",
		fileName.get());
#endif
	
	FILE* file;
#ifdef IOCP_SERVER
	fopen_s(&file, fileDir.get(), "a");
#else
	file = fopen(fileDir.get(), "a");
#endif
	std::shared_ptr<char> logg = std::shared_ptr<char>(new char[6 + strlen(_message) + strlen(_level) + strlen(curDate.get())], std::default_delete<char[]>());
	// 로그 레벨 : 로그 내용 적용시간
#ifdef IOCP_SERVER
	sprintf_s(logg.get() , 6 + strlen(_message) + strlen(_level) + strlen(curDate.get()),
		"%s : %s %s\n",
		_level, _message, curDate.get());
#else
	sprintf(logg.get(), "%s : %s %s\n", _level, _message, curDate.get());
#endif

	fwrite(logg.get(), 1, 5 + strlen(_message) + strlen(_level) + strlen(curDate.get()), file);

	fclose(file);

	std::cout << logg.get();

	return false;
}
