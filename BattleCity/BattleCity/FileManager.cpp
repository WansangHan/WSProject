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
	char* curDate = new char[20];
	currentDateTime(curDate);
	char* fileDir = new char[20];
	char* fileName = new char[11];
	memcpy(fileName, curDate, 10);
	fileName[10] = '\0';
	sprintf_s(fileDir, 20,
		"logs/%s.txt",
		fileName);
	
	FILE* file;
	fopen_s(&file, fileDir, "a");
	
	char* logg = new char[6 + strlen(_message) + strlen(_level) + strlen(curDate)];
	sprintf_s(logg , 6 + strlen(_message) + strlen(_level) + strlen(curDate),
		"%s : %s %s\n",
		_level, _message, curDate);

	fwrite(logg, 1, 5 + strlen(_message) + strlen(_level) + strlen(curDate), file);

	fclose(file);

	std::cout << logg << std::endl;

	delete[] logg;
	delete[] fileDir;
	delete[] fileName;
	delete[] curDate;
	return false;
}
