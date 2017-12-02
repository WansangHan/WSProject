#ifndef INGAME_H
#define INGAME_H
#include <mutex>
#include <memory>
#include "Player.h"
#include "AIObject.h"
#include "PacketManager.h"

enum class SendPacketType : int;

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

	void EnterPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
	void SuccessEnterCalc(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
	void ExitPlayer(std::shared_ptr<CBaseSocket> _sock);
	
	void ApplyPlayerUDP(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
	void ApplyPlayerPositionScale(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
	void ApplyAIObjectPositionScale(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);

	void CollisionNotify(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
};
#endif