#pragma once
class CPlayer
{
	int m_ID;
public:
	CPlayer();
	~CPlayer();

	void SetID(int _ID) { m_ID = _ID; }
	int GetID() { return m_ID; }
};

