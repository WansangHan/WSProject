#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "BaseSocket.h"

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

struct PlayerTransform
{
	float m_vectorX;
	float m_vectorY;
	float m_scale;
	PlayerDirection m_dir;
	PlayerTransform(){}
	PlayerTransform(float _vectorX, float _vectorY, float _scale, int _dir)
	{
		m_vectorX = _vectorX;
		m_vectorY = _vectorY;
		m_scale = _scale;
		m_dir = (PlayerDirection)_dir;
	}
};

class CPlayer
{
	int m_id;
	std::string m_name;
	std::shared_ptr<CBaseSocket> m_sock;

	PlayerTransform m_playerTransform;
public:
	CPlayer();
	~CPlayer();

	void SetID(int _id) { m_id = _id; }
	int GetID() { return m_id; }
	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }
	void SetSocket(std::shared_ptr<CBaseSocket> _sock) { m_sock = _sock; }
	std::shared_ptr<CBaseSocket> GetSocket() { return m_sock; }

	void SetTransform(PlayerTransform _playerTransform) 
	{ 
		m_playerTransform.m_vectorX = _playerTransform.m_vectorX;
		m_playerTransform.m_vectorY = _playerTransform.m_vectorY;
		m_playerTransform.m_scale = _playerTransform.m_scale;
		m_playerTransform.m_dir = _playerTransform.m_dir;
	}
	PlayerTransform GetTransform() { return m_playerTransform; }
};
#endif