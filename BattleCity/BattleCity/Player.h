#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "PacketManager.h"

struct ObjectTransform;

class CPlayer
{
	int m_ID;
	std::string m_name;

	std::shared_ptr<ObjectTransform> m_playerTransform;
public:
	CPlayer();
	~CPlayer();

	void SetID(int _ID) { m_ID = _ID; }
	int GetID() { return m_ID; }
	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }

	void SetTransform(std::shared_ptr<ObjectTransform> _playerTransform)
	{
		m_playerTransform = _playerTransform;
	}
	std::shared_ptr<ObjectTransform> GetTransform() { return m_playerTransform; }

	void PaintPlayer(HWND _hwnd, HDC _hdc);
	void KeyCheck();
	void PlayerMove();
};
#endif