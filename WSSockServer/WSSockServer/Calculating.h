#ifndef CALCULATING_H
#define CALCULATING_H
#include <stdlib.h>
#include "PacketManager.h"
#include "Player.h"
#include "AIObject.h"

enum class SendPacketType : int;
class CPlayer;
class CAIObject;

// �÷��̾� ���� ���°�
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

struct ObjectTransform
{
	float m_vectorX;
	float m_vectorY;
	float m_scale;
	float m_speed;
	ObjectDirection m_dir;
	ObjectTransform() {}
	ObjectTransform(float _vectorX, float _vectorY, float _scale, float _speed, ObjectDirection _dir)
	{
		m_vectorX = _vectorX;
		m_vectorY = _vectorY;
		m_scale = _scale;
		m_speed = _speed;
		m_dir = _dir;
	}
	~ObjectTransform() {}
};

class CCalculating
{
	static std::unique_ptr<CCalculating> m_inst;
	static std::once_flag m_once;

	// ���� ���� Ŭ���̾�Ʈ�� �����ϱ� ���� map ����
	std::map<int, std::shared_ptr<CPlayer>> m_players;

	// ���� Thread �Լ�
	std::unique_ptr<std::thread> m_calculate_Thread;
	bool isContinue;

	CCalculating();
	std::shared_ptr<CPlayer> FindPlayerToID(int _pID);
public:
	static CCalculating& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CCalculating()); });		// �� ���� ȣ��ǰ� �ϱ� ���� call_once
		return *m_inst;
	}
	~CCalculating();

	void InitCalculating();
	void CalculateAll();

	void SetStartingPosition(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);

	void EnterPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
	void ApplyPlayerSocket(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
	void ApplyPlayerUDP(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
	void ExitPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);

	void ApplyPlayerTrasform(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
};
#endif