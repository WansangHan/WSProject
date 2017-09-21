#ifndef PLAYMANAGER_H
#define PLAYMANAGER_H
#include <list>
#include "PacketManager.h"
#include "Player.h"
#include "AIObject.h"

class CGameManager;
class CPlayer;
class CAIObject;

// 오브젝트 방향 상태값
enum ObjectDirection
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

// 오브젝트 Transform
struct ObjectTransform
{
	float m_vectorX;
	float m_vectorY;
	float m_scale;
	ObjectDirection m_dir;
	ObjectTransform() {}
	ObjectTransform(float _vectorX, float _vectorY, float _scale, int _dir)
	{
		m_vectorX = _vectorX;
		m_vectorY = _vectorY;
		m_scale = _scale;
		m_dir = (ObjectDirection)_dir;
	}
};

class CPlayManager
{
	std::shared_ptr<CPlayer> m_ownPlayer;		// 클라이언트가 플레이하는 CPlayer 클래스 변수
	std::list<std::shared_ptr<CPlayer>> m_otherPlayer;	// 상대 플레이어 CPlayer 클래스 리스트
	std::list<std::shared_ptr<CAIObject>> m_AIObjects;	// AIObject 클래스 리스트

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
	void SetPlayerPositionScale(char* _data, int _size);
	void SetAIObjectPositionScale(char* _data, int _size);
	void CheckKey();

	std::shared_ptr<CPlayer> GetOwnPlayer() { return m_ownPlayer; }
};
#endif