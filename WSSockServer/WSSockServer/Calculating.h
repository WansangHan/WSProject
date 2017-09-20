#ifndef CALCULATING_H
#define CALCULATING_H
#include <stdlib.h>
#include "PacketManager.h"

// 플레이어 방향 상태값
enum class ObjectDirection : int
{
	IDLE = 100,
	UPUP,
	LEFT,
	RGHT,
	DOWN,
	UPLE,
	UPRG,
	DWLE,
	DWRG,
};

class CCalculating
{
	static std::unique_ptr<CCalculating> m_inst;
	static std::once_flag m_once;

	CCalculating();
public:
	static CCalculating& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CCalculating()); });		// 한 번만 호출되게 하기 위한 call_once
		return *m_inst;
	}
	~CCalculating();

	void InitCalculating();
	void SetStartingPosition(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size);
};
#endif