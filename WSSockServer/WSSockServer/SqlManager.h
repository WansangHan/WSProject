#pragma once
#pragma comment(lib, "libmysql.lib")
#include <my_global.h>
#include <mysql.h>
#include <iostream>

class CSqlManager
{
	MYSQL m_conn;
	MYSQL *m_connection;
public:
	CSqlManager();
	~CSqlManager();
	bool InitSQLManager();
};

