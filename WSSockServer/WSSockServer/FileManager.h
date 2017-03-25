#pragma once
class CFileManager
{
public:
	CFileManager();
	~CFileManager();

	bool WriteFileLog(char* _message, char* _level);
};

