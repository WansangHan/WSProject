#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "BaseSocket.h"
#ifdef IOCP_SERVER
#include "InGame.h"
#else
#include "Calculating.h"
#endif

struct ObjectTransform;

class CPlayer
{
	int m_id;
	std::string m_name;

	std::shared_ptr<CBaseSocket> m_sock;
	sockaddr_in m_addr;

	std::shared_ptr<ObjectTransform> m_playerTransform;
public:
	CPlayer();
	~CPlayer();

	void SetID(int _id) { m_id = _id; }
	int GetID() { return m_id; }
	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }
	
	void SetSocket(std::shared_ptr<CBaseSocket> _sock) { m_sock = _sock; }
	std::shared_ptr<CBaseSocket> GetSocket() { return m_sock; }
	void SetAddr(sockaddr_in _addr) { m_addr = _addr; }
	sockaddr_in GetAddr() { return m_addr; }

	void SetTransform(std::shared_ptr<ObjectTransform> _playerTransform) { m_playerTransform = _playerTransform; }
	std::shared_ptr<ObjectTransform> GetTransform() { return m_playerTransform; }
};
#endif