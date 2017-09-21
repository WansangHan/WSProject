#pragma once
#include "PlayManager.h"

struct ObjectTransform;

class CAIObject
{
	int m_ID;

	std::shared_ptr<ObjectTransform> m_aiObjectTransform;
public:
	CAIObject();
	~CAIObject();

	void SetID(int _ID) { m_ID = _ID; }
	int GetID() { return m_ID; }

	void SetTransform(std::shared_ptr<ObjectTransform> _playerTransform)
	{
		m_aiObjectTransform = _playerTransform;
	}
	std::shared_ptr<ObjectTransform> GetTransform() { return m_aiObjectTransform; }
	void PaintAIObject(HWND _hwnd, HDC _hdc);
};

