#pragma once
#include "PacketManager.h"
class CInGame
{
	static std::unique_ptr<CInGame> m_inst;
	static std::once_flag m_once;

	CInGame();
public:
	static CInGame& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CInGame()); });
		return *m_inst;
	}
	~CInGame();

	void EnterPlayer(std::shared_ptr<CBaseSocket> _sock, char* _data);
};

