#pragma once
#include <functional>
#include <map>
#include <atlstr.h>
#include "CommonUIScene.h"
#include "LoginUIScene.h"
#include "GameManager.h"
#include "LogManager.h"

class CCommonUIScene;
class CLoginUIScene;

// Scene 상태 enum
enum SceneState
{
	LOGIN,
};

// WM_COMMAND에서 콜백 호출 시 윈도우 오브젝트들을 구분하는 ENUM
enum HMENUNUMBER
{
	COMMON_EXIT_BUTTON,

	NEWACCOUNT_ID_BOX,
	NEWACCOUNT_PW_BOX,
	NEWACCOUNT_ML_BOX,
	NEWACCOUNT_NEWACCOUNT_BUTTON,

	LOGIN_ID_BOX,
	LOGIN_PW_BOX,
	LOGIN_LOGIN_BUTTON,
};

class CUIManager
{
	SceneState m_sceneState;

	CCommonUIScene* m_commonUIScene;
	CLoginUIScene* m_loginUIScene;

	// 윈도우 메시지 콜백에 대응하기 위한 상태값 <-> 함수포인터 매칭 map
	typedef std::function<void(HWND, HDC)> Paint_Function;
	std::map < SceneState, Paint_Function > Paint_functionmap;

	typedef std::function<void(HWND, LPARAM)> MouceLButtonDown_Function;
	std::map < SceneState, MouceLButtonDown_Function > MouceLButtonDown_functionmap;

	typedef std::function<void(HWND, WPARAM)> CommandHandling_Function;
	std::map < HMENUNUMBER, CommandHandling_Function > CommandHandling_functionmap;

public:
	CUIManager();
	~CUIManager();

	void ChangeScene(HWND _hwnd, SceneState _sceneState);

	bool InitUIManager();
	void InitFunction();
	void PaintUI(HWND _hwnd, HDC _hdc);
	void CommandHandling(HWND _hwnd, WPARAM wParam);
	void MouceLButtonDown(HWND _hwnd, LPARAM lParam);
};

