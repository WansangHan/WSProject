#ifndef BASEOBJECT_H
#define BASEOBJECT_H
#include <iostream>
#include <memory>
#include <math.h>

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

class CBaseObject
{
	int m_id;

	std::shared_ptr<ObjectTransform> m_objectTransform;
#ifndef IOCP_SERVER
	std::shared_ptr<ObjectTransform> m_curPosition;
	unsigned int m_lastGetTickCount;
#endif
public:
	CBaseObject();
	~CBaseObject();

	void SetID(int _id) { m_id = _id; }
	int GetID() { return m_id; }

	void SetTransform(std::shared_ptr<ObjectTransform> _playerTransform) { m_objectTransform = _playerTransform; }
	std::shared_ptr<ObjectTransform> GetTransform() { return m_objectTransform; }
#ifndef IOCP_SERVER
	void SetCurTransform(std::shared_ptr<ObjectTransform> _curPosition) { m_curPosition = _curPosition; }
	std::shared_ptr<ObjectTransform> GetCurTransform() { return m_curPosition; }
	void SetLastGetTickCount(unsigned int _lastGetTickCount) { m_lastGetTickCount = _lastGetTickCount; }
	unsigned int GetLastGetTickCount() { return m_lastGetTickCount; }
	void CalculateCurrentPosition();
#endif
};
#endif