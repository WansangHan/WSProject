#include "stdafx.h"
#include "FileManager.h"
#include <iostream>

CFileManager::CFileManager()
{
}


CFileManager::~CFileManager()
{
}


bool CFileManager::WriteFileLog(const char * _message, char * _level)
{
	std::shared_ptr<char> curDate = std::shared_ptr<char>(new char[20], std::default_delete<char[]>());
	currentDateTime(curDate.get());
	std::shared_ptr<char> fileDir = std::shared_ptr<char>(new char[20], std::default_delete<char[]>());
	std::shared_ptr<char> fileName = std::shared_ptr<char>(new char[11], std::default_delete<char[]>());
	memcpy(fileName.get(), curDate.get(), 10);
	fileName.get()[10] = '\0';
	sprintf_s(fileDir.get(), 20,
		"logs/%s.txt",
		fileName.get());
	
	FILE* file;
	fopen_s(&file, fileDir.get(), "a");
	
	std::shared_ptr<char> logg = std::shared_ptr<char>(new char[6 + strlen(_message) + strlen(_level) + strlen(curDate.get())], std::default_delete<char[]>());
	sprintf_s(logg.get() , 6 + strlen(_message) + strlen(_level) + strlen(curDate.get()),
		"%s : %s %s\n",
		_level, _message, curDate.get());

	fwrite(logg.get(), 1, 5 + strlen(_message) + strlen(_level) + strlen(curDate.get()), file);

	fclose(file);

	return false;
}
