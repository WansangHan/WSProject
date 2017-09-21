#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
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

	bool isConnected;	// ������ ���� �Ǿ��ִ� ��

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

	void EnterGame();
	void EnterPlayer(char* _data, int _size);
	void ExitPlayer(char* _data, int _size);
	void SetPlayerPositionScale(char* _data, int _size);
	void SetAIObjectPositionScale(char* _data, int _size);

	CPlayManager* GetPlayerManagerInstance() { return m_playManager; }
	CUIManager* GetUIManagerInstance() { return m_uiManager; }
};
#endif