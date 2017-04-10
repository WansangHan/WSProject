#pragma once
class CFileManager
{
public:
	CFileManager();
	~CFileManager();

	bool WriteFileLog(const char* _message, char* _level);
};

