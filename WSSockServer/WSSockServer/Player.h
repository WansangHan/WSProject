#pragma once
#include <string>
#include "BaseSocket.h"
class CPlayer
{
	int m_id;
	std::string m_name;
	std::shared_ptr<CBaseSocket> m_sock;

	float m_vectorX;
	float m_vectorY;
	int m_dir;
	float m_scale;
public:
	CPlayer();
	~CPlayer();

	void SetID(int _id) { m_id = _id; }
	int GetID() { return m_id; }
	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }
	void SetSocket(std::shared_ptr<CBaseSocket> _sock) { m_sock = _sock; }
	std::shared_ptr<CBaseSocket> GetSocket() { return m_sock; }

	void SetXY(float _vectorX, float _vectorY) { m_vectorX = _vectorX; m_vectorY = _vectorY; }
	float GetX() { return m_vectorX; }
	float GetY() { return m_vectorY; }
	void SetDir(int _dir) { m_dir = _dir; }
	int GetDir() { return m_dir; }
	void SetScale(float _scale) { m_scale = _scale; }
	float GetScale() { return m_scale; }
};

