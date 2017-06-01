#pragma once
#pragma comment(lib, "libprotobufd")
#include "LogManager.h"
#include "NetworkManager.h"
#include "PacketManager.h"

class CNetWorkManager;

class CGameManager
{
	static std::unique_ptr<CGameManager> m_inst;
	static std::once_flag m_once;
	CGameManager();
public:
	static CGameManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CGameManager()); });
		return *m_inst;
	}
	~CGameManager();

	bool InitGameManager();

};

