#pragma once
#pragma comment(lib, "libmysql.lib")
#include <my_global.h>
#include <mysql.h>
#include <iostream>
#include <stdio.h>

class CSqlManager
{
	MYSQL m_conn;
	MYSQL *m_connection;
public:
	CSqlManager();
	~CSqlManager();
	bool InitSQLManager();
	bool SendLogMessage(char* _message, char* _level);
};

