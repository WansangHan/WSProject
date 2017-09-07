#pragma once
#include "PacketManager.h"
#include "Player.h"

// �÷��̾� ���� ���°�
enum PlayerDirection
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

class CInGame
{
	static std::unique_ptr<CInGame> m_inst;
	static std::once_flag m_once;

	// ���� ���� Ŭ���̾�Ʈ�� �����ϱ� ���� map ����
	std::map<int, std::shared_ptr<CPlayer>> m_players;

	CInGame();
	int FindIDToSOCKET(std::shared_ptr<CBaseSocket> _sock);
	std::shared_ptr<CPlayer> FindPlayerToID(int _pID);
	void SetStartingPositionScale(std::shared_ptr<CPlayer> _player);
public:
	static CInGame& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CInGame()); });
		return *m_inst;
	}
	~CInGame();

	void EnterPlayer(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size);
	void ExitPlayer(std::shared_ptr<CBaseSocket> _sock);
};

