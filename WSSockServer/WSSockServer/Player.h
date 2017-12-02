#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "BaseObject.h"
#include "BaseSocket.h"

class CPlayer : public CBaseObject
{
#ifndef IOCP_SERVER
	std::string m_name;
#endif

	std::shared_ptr<CBaseSocket> m_sock;
	sockaddr_in m_addr;
public:
	CPlayer();
	~CPlayer();

#ifndef IOCP_SERVER
	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }
#endif
	
	void SetSocket(std::shared_ptr<CBaseSocket> _sock) { m_sock = _sock; }
	std::shared_ptr<CBaseSocket> GetSocket() { return m_sock; }
	void SetAddr(sockaddr_in _addr) { m_addr = _addr; }
	sockaddr_in GetAddr() { return m_addr; }
};
#endif