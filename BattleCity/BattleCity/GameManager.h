#pragma once
#pragma comment(lib, "libprotobufd")
#include "LogManager.h"
#include "NetworkManager.h"
#include "PacketManager.h"
#include "UIManager.h"
#include "PlayManager.h"

enum GameState
{
	INTRO,
	PLAY,
};

class CNetWorkManager;
class CUIManager;
class CPlayManager;

class CGameManager
{
	static std::unique_ptr<CGameManager> m_inst;
	static std::once_flag m_once;

	CUIManager* m_uiManager;
	CPlayManager* m_playManager;

	HWND m_hwnd;
	HDC m_hdc;
	PAINTSTRUCT ps;

	GameState m_gameState;

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
	bool PaintAll();
	void CommandHandling(HWND _hwnd, WPARAM _wParam);
	void LButtonDown(LPARAM lParam);

	void EnterGame();

	CPlayManager* GetPlayerManagerInstance() { return m_playManager; }
	CUIManager* GetUIManagerInstance() { return m_uiManager; }
};

