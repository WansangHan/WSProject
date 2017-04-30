#include "stdafx.h"
#include "SqlManager.h"


CSqlManager::CSqlManager()
{
	mysql_init(&m_conn);
}


CSqlManager::~CSqlManager()
{
}

bool CSqlManager::InitSQLManager()
{
	m_connection = mysql_real_connect(
		&m_conn,
		"192.168.127.128", 
		"WS", "@013meadmin", 
		"WSDB", 3306, (char*)NULL, 0);

	if (m_connection == NULL)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", false, "Database Connect Error");
		return false;
	}
	return true;
}

bool CSqlManager::SendLogMessage(const char * _message, char * _level)
{
	char* query = new char[53 + strlen(_message) + strlen(_level)];
#ifdef IOCP_SERVER
	sprintf_s(query, 53 + strlen(_message) + strlen(_level),
		"SELECT INSERT_LOG_MESSAGE('%s', '%s', '%s') FROM DUAL;",
		SERVER_CATE, _level, _message);
#else
	sprintf(query,
		"SELECT INSERT_LOG_MESSAGE('%s', '%s', '%s') FROM DUAL;",
		SERVER_CATE, _level, _message);
#endif

	MYSQL_RES *sql_result;
	int query_result;

	query_result = mysql_query(m_connection, query);
	if (query_result != 0)
		CLogManager::getInstance().WriteLogMessage("ERROR", false, "Database Query Error");

	sql_result = mysql_store_result(m_connection);

	delete[] query;
	return false;
}
