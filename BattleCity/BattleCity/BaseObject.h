#pragma once
#include <mutex>
#include <memory>

// ������Ʈ ���� ���°�
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

// ������Ʈ Transform
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

	// �÷��̾��� ������ �̵� ����
	std::shared_ptr<ObjectTransform> m_objectTransform;

	// �÷��̾��� ���� ��ǥ
	std::shared_ptr<ObjectTransform> m_curPosition;
	// ������ �̵� ������ ����� �ð�
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