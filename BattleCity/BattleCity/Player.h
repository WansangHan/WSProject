#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "BaseObject.h"

class CPlayer : public CBaseObject
{
	std::string m_name;
public:
	CPlayer();
	~CPlayer();

	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }

	void KeyCheck();
	void PlayerMove();
};
#endif