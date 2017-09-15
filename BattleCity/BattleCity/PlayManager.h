#ifndef PLAYMANAGER_H
#define PLAYMANAGER_H
#include <list>
#include "PacketManager.h"
#include "Player.h"

class CGameManager;
class CPlayer;

class CPlayManager
{
	std::shared_ptr<CPlayer> m_ownPlayer;		// 클라이언트가 플레이하는 CPlayer 클래스 변수
	std::list<std::shared_ptr<CPlayer>> m_otherPlayer;	// 상대 플레이어 CPlayer 클래스 리스트

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