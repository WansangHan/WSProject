#pragma once

class CIOCP
{
	WSADATA m_wsaData;
public:
	CIOCP();
	~CIOCP();

	bool InitServer();
	void Update();
	void CloseServer();
};

