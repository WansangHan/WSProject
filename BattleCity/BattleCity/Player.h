#pragma once
#include <string>
class CPlayer
{
	int m_ID;
	std::string m_name;
public:
	CPlayer();
	~CPlayer();

	void SetID(int _ID) { m_ID = _ID; }
	int GetID() { return m_ID; }
	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }
};

