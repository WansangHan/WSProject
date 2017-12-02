#ifndef AIOBJECT_H
#define AIOBJECT_H
#include <mutex>
#ifdef IOCP_SERVER
#include "Calculating.cuh"
#else
#include "InGame.h"
#endif

struct ObjectTransform;

class CAIObject
{
	int m_id;

	std::shared_ptr<ObjectTransform> m_objectTransform;
#ifndef IOCP_SERVER
	std::shared_ptr<ObjectTransform> m_curPosition;
	unsigned int m_lastGetTickCount;
#endif
public:
	CAIObject();
	~CAIObject();

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