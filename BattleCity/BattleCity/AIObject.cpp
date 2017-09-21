#include "stdafx.h"
#include "AIObject.h"


CAIObject::CAIObject()
{
	m_aiObjectTransform = std::make_shared<ObjectTransform>();
}


CAIObject::~CAIObject()
{
}

void CAIObject::PaintAIObject(HWND _hwnd, HDC _hdc)
{
	int left = m_aiObjectTransform->m_vectorX - m_aiObjectTransform->m_scale / 2;
	int top = m_aiObjectTransform->m_vectorY - m_aiObjectTransform->m_scale / 2;
	int right = m_aiObjectTransform->m_vectorX + m_aiObjectTransform->m_scale / 2;
	int bottom = m_aiObjectTransform->m_vectorY + m_aiObjectTransform->m_scale / 2;
	Ellipse(_hdc, left, top, right, bottom);
}
