#include "stdafx.h"
#include "BaseObject.h"


CBaseObject::CBaseObject()
{
	m_objectTransform = std::make_shared<ObjectTransform>();
#ifdef IOCP_SERVER
	m_curPosition = std::make_shared<ObjectTransform>();
#endif
}


CBaseObject::~CBaseObject()
{
}
