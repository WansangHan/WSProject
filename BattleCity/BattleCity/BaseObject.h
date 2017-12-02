#pragma once
#include <mutex>
#include <memory>

// 오브젝트 방향 상태값
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

// 오브젝트 Transform
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
};

class CBaseObject
{
protected:
	int m_ID;

	// 플레이어의 마지막 이동 정보
	std::shared_ptr<ObjectTransform> m_objectTransform;

	// 플레이어의 실제 좌표
	std::shared_ptr<ObjectTransform> m_curPosition;
	// 마지막 이동 정보가 저장된 시간
	unsigned int m_lastGetTickCount;
public:
	CBaseObject();
	~CBaseObject();

	void SetID(int _ID) { m_ID = _ID; }
	int GetID() { return m_ID; }

	void SetTransform(ObjectTransform _objectTransform) { memcpy(m_objectTransform.get(), &_objectTransform, sizeof(ObjectTransform)); }
	std::shared_ptr<ObjectTransform> GetTransform() { return m_objectTransform; }
	void SetCurTransform(ObjectTransform _curPosition) { memcpy(m_curPosition.get(), &_curPosition, sizeof(ObjectTransform));}
	std::shared_ptr<ObjectTransform> GetCurTransform() { return m_curPosition; }
	void SetLastGetTickCount(unsigned int _lastGetTickCount) { m_lastGetTickCount = _lastGetTickCount; }
	unsigned int GetLastGetTickCount() { return m_lastGetTickCount; }

	void PaintObject(HWND _hwnd, HDC _hdc);
};