#ifndef UDPSOCKET_H
#define UDPSOCKET_H
#include "BaseSocket.h"
class CUDPSocket :
	public CBaseSocket
{
public:
	CUDPSocket();
	~CUDPSocket();
};
#endif