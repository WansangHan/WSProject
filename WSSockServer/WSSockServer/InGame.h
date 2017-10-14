#ifndef INGAME_H
#define INGAME_H
#include "PacketManager.h"
#include "Player.h"
#include "AIObject.h"

enum class SendPacketType : int;
class CPlayer;
class CAIObject;

// 플레이어 방향 상태값
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

struct ObjectTransform
{
	float m_vectorX;
	float m_vectorY;
	float m_scale;
	ObjectDirection m_dir;
	ObjectTransform() {}
	ObjectTransform(float _vectorX, float _vectorY, float _scale, ObjectDirection _dir)
	{
		m_vectorX = _vectorX;
		m_vectorY = _vectorY;
		m_scale = _scale;
		m_dir = _dir;
	}
};

class CInGame
{
	static std::unique_ptr<CInGame> m_inst;
	static std::once_flag m_once;

	// 접속 중인 클라이언트를 저장하기 위한 map 변수
	std::map<int, std::shared_ptr<CPlayer>> m_players;

	// 생성된 AI Object를 저장하기 위한 map 변수
	std::map<int, std::shared_ptr<CAIObject>> m_AIObjects;

	CInGame();
	int FindIDToSOCKET(std::shared_ptr<CBaseSocket> _sock);
	std::shared_ptr<CPlayer> FindPlayerToID(int _pID);
	std::shared_ptr<CAIObject> FindAIObjectToID(int _pID);
	int MakeAIObjectID();
	void SendToAllPlayer(SendPacketType _type, std::string _str, sockaddr_in* _sockaddr, bool _isTCP);
	void AllocateAIObject();
public:
	static CInGame& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CInGame()); });
		return *m_inst;
	}
	~CInGame();

	void InitInGame();

	void EnterPlayer(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size);
	void SuccessEnterEpoll(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size);
	void ExitPlayer(std::shared_ptr<CBaseSocket> _sock);
	
	void ApplyPlayerPositionScale(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size);
	void ApplyAIObjectPositionScale(std::shared_ptr<CBaseSocket> _sock, char* _data, int _size);
};
#endif