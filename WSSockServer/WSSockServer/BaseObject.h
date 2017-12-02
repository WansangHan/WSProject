#pragma once
#include <memory>

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
	ObjectTransform()
	{
		memset(this, 0, sizeof(ObjectTransform));
	}
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
protected:
	int m_id;

	std::shared_ptr<ObjectTransform> m_objectTransform;
#ifdef IOCP_SERVER
	std::shared_ptr<ObjectTransform> m_curPosition;
	unsigned int m_lastGetTickCount;
#endif
public:
	CBaseObject();
	~CBaseObject();

	void SetID(int _id) { m_id = _id; }
	int GetID() { return m_id; }

	void SetTransform(ObjectTransform _playerTransform)
	{
		memcpy(m_objectTransform.get(), &_playerTransform, sizeof(ObjectTransform));
	}
	std::shared_ptr<ObjectTransform> GetTransform() { return m_objectTransform; }

#ifdef IOCP_SERVER
	void SetCurTransform(ObjectTransform _curPosition)
	{
		memcpy(m_curPosition.get(), &_curPosition, sizeof(ObjectTransform));
	}
	std::shared_ptr<ObjectTransform> GetCurTransform() { return m_curPosition; }
	void SetLastGetTickCount(unsigned int _lastGetTickCount) { m_lastGetTickCount = _lastGetTickCount; }
	unsigned int GetLastGetTickCount() { return m_lastGetTickCount; }
#endif
};

