#pragma once
#pragma comment(lib, "libprotobufd")
#include "LogManager.h"
#include "NetworkManager.h"
#include "PacketManager.h"
#include "UIManager.h"
#include "PlayManager.h"

// ���� ���°�
enum GameState
{
	INTRO,
	PLAY,
};

// �÷��̾� ���� ���°�
enum PlayerDirection
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

class CNetWorkManager;
class CUIManager;
class CPlayManager;

class CGameManager
{
	static std::unique_ptr<CGameManager> m_inst;
	static std::once_flag m_once;

	CUIManager* m_uiManager;
	CPlayManager* m_playManager;

	HWND m_hwnd;	// ���α׷� ���� �ڵ�
	HDC m_hdc;		// ���α׷� ���� HDC
	PAINTSTRUCT ps;	// ���α׷� ���� PAINTSTRUCT

	GameState m_gameState;		// ���� ����

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
	void CommandHandling(HWND _hwnd, WPARAM _wParam);
	void LButtonDown(LPARAM lParam);

	void EnterGame();
	void SetPositionScale(char* _data, int _size);

	CPlayManager* GetPlayerManagerInstance() { return m_playManager; }
	CUIManager* GetUIManagerInstance() { return m_uiManager; }
};

