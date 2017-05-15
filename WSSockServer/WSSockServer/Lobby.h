#pragma once
#include "LogManager.h"
#include "IOCP.h"
#include "PacketManager.h"
#include "SumTest.pb.h"

class CLobby
{
	static std::unique_ptr<CLobby> m_inst;
	static std::once_flag m_once;

	CLobby();
public:
	static CLobby& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CLobby()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CLobby();

	void SumData(CBaseSocket* sock, char* message);
};

