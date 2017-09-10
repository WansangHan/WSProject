#pragma once
#include <string>
#include "PacketManager.h"

// 플레이어 방향 상태값
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

class CPlayer
{
	int m_ID;
	std::string m_name;

	float m_vectorX;
	float m_vectorY;
	float m_scale;
	int m_dir;
public:
	CPlayer();
	~CPlayer();

	void SetID(int _ID) { m_ID = _ID; }
	int GetID() { return m_ID; }
	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }
	
	void SetXY(float _vectorX, float _vectorY) { m_vectorX = _vectorX; m_vectorY = _vectorY; }
	float GetX() { return m_vectorX; }
	float GetY() { return m_vectorY; }
	void SetDir(int _dir) { m_dir = _dir; }
	int GetDir() { return m_dir; }
	void SetScale(float _scale) { m_scale = _scale; }
	float GetScale() { return m_scale; }

	void PaintPlayer(HWND _hwnd, HDC _hdc);
	void KeyCheck();
	void PlayerMove();
};

