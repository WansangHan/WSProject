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
enum class ObjectDirection : int
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
	float m_speed;
	ObjectDirection m_dir;
	ObjectTransform() {}
	ObjectTransform(float _vectorX, float _vectorY, float _scale, float _speed, ObjectDirection _dir)
	{
		m_vectorX = _vectorX;
		m_vectorY = _vectorY;
		m_scale = _scale;
		m_speed = _speed;
		m_dir = _dir;
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

	void EnterGame(int _id, std::string _name);
	void EnterPlayer(char* _data, int _size);
	void ExitPlayer(char* _data, int _size);
	void SetPlayerPositionScale(char* _data, int _size);
	void SetAIObjectPositionScale(char* _data, int _size);
	void CheckKey();

	std::shared_ptr<CPlayer> GetOwnPlayer() { return m_ownPlayer; }
};
#endif