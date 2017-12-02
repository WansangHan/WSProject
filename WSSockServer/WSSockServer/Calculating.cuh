#ifndef CALCULATING_H
#define CALCULATING_H
#include <mutex>
#include <tbb/concurrent_hash_map.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include "BaseSocket.h"
#include "Player.h"

#ifdef __cplusplus 
extern "C"
{//<-- extern 시작
#endif
	class CCalculating
	{
		static std::unique_ptr<CCalculating> m_inst;
		static std::once_flag m_once;

		// 접속 중인 클라이언트를 저장하기 위한 map 변수
		tbb::concurrent_hash_map<int, std::shared_ptr<CPlayer>> m_players;

		// 연산 Thread 함수
		std::unique_ptr<std::thread> m_calculate_Thread;
		bool isContinue;

		CCalculating();
		std::shared_ptr<CPlayer> FindPlayerToID(int _pID);
	public:
		static CCalculating& getInstance()
		{
			std::call_once(m_once, []() { m_inst.reset(new CCalculating()); });		// 한 번만 호출되게 하기 위한 call_once
			return *m_inst;
		}
		~CCalculating();

		void InitCalculating();
		void CalculateAll();

		void SetStartingPosition(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);

		void EnterPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
		void ApplyPlayerSocket(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
		void ApplyPlayerUDP(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
		void ExitPlayer(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);

		void ApplyPlayerTrasform(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
		void IncreaseScale(std::shared_ptr<CBaseSocket> _sock, sockaddr_in _addr, char* _data, int _size);
	};
#ifdef __cplusplus 
}
#endif
#endif

