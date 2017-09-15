#ifndef PLAYMANAGER_H
#define PLAYMANAGER_H
#include <list>
#include "PacketManager.h"
#include "Player.h"

class CGameManager;
class CPlayer;

class CPlayManager
{
	std::shared_ptr<CPlayer> m_ownPlayer;		// Ŭ���̾�Ʈ�� �÷����ϴ� CPlayer Ŭ���� ����
	std::list<std::shared_ptr<CPlayer>> m_otherPlayer;	// ��� �÷��̾� CPlayer Ŭ���� ����Ʈ

	std::shared_ptr<CPlayer> FindPlayerToID(int _pID);
public:
	CPlayManager();
	~CPlayManager();

	void InitPlayerManager();
	void PaintPlay(HWND _hwnd, HDC _hdc);
	void UpdatePlay();

	void EnterGame();
	void EnterPlayer(char* _data, int _size);
	void ExitPlayer(char* _data, int _size);
	void SetPositionScale(char* _data, int _size);
	void CheckKey();

	std::shared_ptr<CPlayer> GetOwnPlayer() { return m_ownPlayer; }
};
#endif