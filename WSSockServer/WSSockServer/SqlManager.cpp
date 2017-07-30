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
	// Mysql 서버 접속
	m_connection = mysql_real_connect(
		&m_conn,
		"192.168.68.128", 
		"WS", "@013meadmin", 
		"WSDB", 3306, (char*)NULL, 0);

	if (m_connection == NULL)
	{
		CLogManager::getInstance().WriteLogMessage("ERROR", false, "Database Connect Error");
		return false;
	}

	CLogManager::getInstance().WriteLogMessage("INFO", true, "Database Connect Success");
	return true;
}

// 로그 메시지를 디비에 저장하는 함수
bool CSqlManager::SendLogMessage(const char * _message, char * _level)
{
	std::shared_ptr<char> query = std::shared_ptr<char>(new char[53 + strlen(_message) + strlen(_level)], std::default_delete<char[]>());
	// 쿼리에 넘어온 데이터 합치기
#ifdef IOCP_SERVER
	sprintf_s(query.get(), 53 + strlen(_message) + strlen(_level),
		"SELECT INSERT_LOG_MESSAGE('%s', '%s', '%s') FROM DUAL;",
		SERVER_CATE, _level, _message);
#else
	sprintf(query.get(),
		"SELECT INSERT_LOG_MESSAGE('%s', '%s', '%s') FROM DUAL;",
		SERVER_CATE, _level, _message);
#endif

	MYSQL_RES *sql_result;
	int query_result;

	query_result = mysql_query(m_connection, query.get());
	if (query_result != 0)
		CLogManager::getInstance().WriteLogMessage("ERROR", false, "Database Query Error : %d %s", query_result, mysql_error(m_connection));

	sql_result = mysql_store_result(m_connection);

	return false;
}
