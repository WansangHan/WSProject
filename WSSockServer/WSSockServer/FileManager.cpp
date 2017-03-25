#include "stdafx.h"
#include "FileManager.h"
#include <iostream>

CFileManager::CFileManager()
{
}


CFileManager::~CFileManager()
{
}


bool CFileManager::WriteFileLog(char * _message, char * _level)
{
	char* curDate = new char[20];
	currentDateTime(curDate);
	
	char fileName[20] = "logs/";
	char filetail[5] = ".txt";
	memcpy(fileName + 5, curDate, 10);
	strcat_s(fileName, filetail);
	
	FILE* file;
	fopen_s(&file, fileName, "a");
	
	char* logg = new char[6 + strlen(_message) + strlen(_level) + strlen(curDate)];
	sprintf_s(logg , 6 + strlen(_message) + strlen(_level) + strlen(curDate),
		"%s : %s %s\n",
		_level, _message, curDate);

	fwrite(logg, 1, 5 + strlen(_message) + strlen(_level) + strlen(curDate), file);

	fclose(file);

	std::cout << logg << std::endl;

	delete[] logg;
	delete[] curDate;
	return false;
}
