#include "stdafx.h"
#include "SqlManager.h"


CSqlManager::CSqlManager()
{
	mysql_init(&m_conn);
}


CSqlManager::~CSqlManager()
{
}

bool CSqlManager::InitConnection()
{
	m_connection = mysql_real_connect(
		&m_conn, 
		"192.168.127.128", 
		"WS", "@013meadmin", 
		"WSDB", 3306, (char*)NULL, 0);

	if (m_connection == NULL)
	{
		std::cout << "Connect Error" << std::endl;
		return false;
	}
	std::cout << "Connect Success" << std::endl;
	return true;
}
