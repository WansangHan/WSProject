#include "stdafx.h"
#include "BaseObject.h"

CBaseObject::CBaseObject()
{
	m_objectTransform = std::make_shared<ObjectTransform>();
	m_curPosition = std::make_shared<ObjectTransform>();
	m_lastGetTickCount = GetTickCount();
}

CBaseObject::~CBaseObject()
{
}

// ��ǥ, ũ�Ⱚ�� ������� �÷��̾ �׸�
void CBaseObject::PaintObject(HWND _hwnd, HDC _hdc)
{
	int left = m_curPosition->m_vectorX - m_objectTransform->m_scale / 2;
	int top = m_curPosition->m_vectorY - m_objectTransform->m_scale / 2;
	int right = m_curPosition->m_vectorX + m_objectTransform->m_scale / 2;
	int bottom = m_curPosition->m_vectorY + m_objectTransform->m_scale / 2;
	Ellipse(_hdc, left, top, right, bottom);
}