#ifndef FILEMANAGER_H
#define FILEMANAGER_H

class CFileManager
{
public:
	CFileManager();
	~CFileManager();

	bool WriteFileLog(const char* _message, char* _level);
};
#endif