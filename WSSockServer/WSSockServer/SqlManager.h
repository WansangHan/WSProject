#ifndef SQLMANAGER_H
#define SQLMANAGER_H
#include <my_global.h>
#include <mysql.h>
#undef min
#undef max
#undef test

class CSqlManager
{
	MYSQL m_conn;
	MYSQL *m_connection;
public:
	CSqlManager();
	~CSqlManager();
	bool InitSQLManager();
	bool SendLogMessage(const char* _message, char* _level);
};
#endif