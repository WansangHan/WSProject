#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "PacketManager.h"

struct ObjectTransform;

class CPlayer
{
	int m_ID;
	std::string m_name;

	// �÷��̾��� ������ �̵� ����
	std::shared_ptr<ObjectTransform> m_playerTransform;

	// �÷��̾��� ���� ��ǥ
	std::shared_ptr<ObjectTransform> m_curPosition;
	// ������ �̵� ������ ����� �ð�
	unsigned int m_lastGetTickCount;
public:
	CPlayer();
	~CPlayer();

	void SetID(int _ID) { m_ID = _ID; }
	int GetID() { return m_ID; }
	void SetName(std::string _name) { m_name = _name; }
	std::string GetName() { return m_name; }

	void SetTransform(std::shared_ptr<ObjectTransform> _playerTransform) { m_playerTransform = _playerTransform; }
	std::shared_ptr<ObjectTransform> GetTransform() { return m_playerTransform; }
	void SetCurTransform(std::shared_ptr<ObjectTransform> _curPosition) { m_curPosition = _curPosition; }
	std::shared_ptr<ObjectTransform> GetCurTransform() { return m_curPosition; }
	void SetLastGetTickCount(unsigned int _lastGetTickCount) { m_lastGetTickCount = _lastGetTickCount; }
	unsigned int GetLastGetTickCount() { return m_lastGetTickCount; }

	void PaintPlayer(HWND _hwnd, HDC _hdc);
	void KeyCheck();
	void PlayerMove();
};
#endif