#pragma once
#include <string>
#include "BaseSocket.h"
class CPlayer
{
	int m_id;
	std::string m_name;
	std::shared_ptr<CTCPSocket> m_sock;
public:
	CPlayer();
	~CPlayer();

	void SetID(int _id) { m_id = _id; }
	int GetID() { return m_id; }
	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }
	void SetSocket(std::shared_ptr<CBaseSocket> _sock) { m_sock = _sock; }
	std::shared_ptr<CBaseSocket> GetSocket() { return m_sock; }
};

