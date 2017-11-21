#ifndef PLAYER_H
#define PLAYER_H
#ifdef IOCP_SERVER
#include "Calculating.cuh"
#else
#include <string>
#include "BaseSocket.h"
#include "InGame.h"
#endif

struct ObjectTransform;

class CPlayer
{
	int m_id;

#ifndef IOCP_SERVER
	std::string m_name;
#endif
	std::shared_ptr<ObjectTransform> m_objectTransform;
#ifdef IOCP_SERVER
	std::shared_ptr<ObjectTransform> m_curPosition;
	unsigned int m_lastGetTickCount;
#endif

	std::shared_ptr<CBaseSocket> m_sock;
	sockaddr_in m_addr;
public:
	CPlayer();
	~CPlayer();

	void SetID(int _id) { m_id = _id; }
	int GetID() { return m_id; }

#ifndef IOCP_SERVER
	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }
#endif

	void SetTransform(ObjectTransform* _playerTransform, int size)
	{
		memcpy(m_objectTransform.get(), _playerTransform, size);
	}
	std::shared_ptr<ObjectTransform> GetTransform() { return m_objectTransform; }

#ifdef IOCP_SERVER
	void SetCurTransform(ObjectTransform* _curPosition, int size)
	{
		memcpy(m_curPosition.get(), _curPosition, size);
	}
	std::shared_ptr<ObjectTransform> GetCurTransform() { return m_curPosition; }
	void SetLastGetTickCount(unsigned int _lastGetTickCount) { m_lastGetTickCount = _lastGetTickCount; }
	unsigned int GetLastGetTickCount() { return m_lastGetTickCount; }
#endif
	
	void SetSocket(std::shared_ptr<CBaseSocket> _sock) { m_sock = _sock; }
	std::shared_ptr<CBaseSocket> GetSocket() { return m_sock; }
	void SetAddr(sockaddr_in _addr) { m_addr = _addr; }
	sockaddr_in GetAddr() { return m_addr; }
};
#endif