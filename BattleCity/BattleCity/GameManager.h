#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "UIManager.h"
#include "PlayManager.h"

// 게임 상태값
enum GameState
{
	INTRO,
	LODDING,
	PLAY,
};

class CGameManager
{
	static std::unique_ptr<CGameManager> m_inst;
	static std::once_flag m_once;

	CUIManager* m_uiManager;
	CPlayManager* m_playManager;

	HWND m_hwnd;	// 프로그램 메인 핸들
	HDC m_hdc;		// 프로그램 메인 HDC
	PAINTSTRUCT ps;	// 프로그램 메인 PAINTSTRUCT

	GameState m_gameState;		// 게임 상태

	bool isConnected;	// 서버와 연결 되어있는 지

	CGameManager();
public:
	static CGameManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CGameManager()); });
		return *m_inst;
	}
	~CGameManager();

	bool InitGameManager(HWND _hwnd);
	void ExitGameManager();
	void Timer(HWND _hwnd);
	bool PaintAll();
	UINT Nchittest(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam);
	void CommandHandling(HWND _hwnd, WPARAM _wParam);
	void LButtonDown(LPARAM lParam);
	void KeyChange();

	void EnterGame(int _id, std::string _name);
	void SyncConnectSuccess(char* _data, int _size);
	void CompleteConnect(char* _data, int _size);
	void EnterPlayer(char* _data, int _size);
	void ExitPlayer(char* _data, int _size);
	void SetPlayerPositionScale(char* _data, int _size);
	void SetAIObjectPositionScale(char* _data, int _size);
	void DeathNotify(char* _data, int _size);
	void IncreaseScale(char* _data, int _size);

	CPlayManager* GetPlayerManagerInstance() { return m_playManager; }
	CUIManager* GetUIManagerInstance() { return m_uiManager; }
};
#endif