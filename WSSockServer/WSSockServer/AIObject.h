#ifndef AIOBJECT_H
#define AIOBJECT_H
#include "PacketManager.h"

struct ObjectTransform;

class CAIObject
{
	int m_id;
	std::shared_ptr<ObjectTransform> m_AITransform;
public:
	CAIObject();
	~CAIObject();

	void SetID(int _id) { m_id = _id; }
	int GetID() { return m_id; }
	void SetTransform(std::shared_ptr<ObjectTransform> _AITransform) { m_AITransform = _AITransform; }
	std::shared_ptr<ObjectTransform> GetTransform() { return m_AITransform; }
};
#endif