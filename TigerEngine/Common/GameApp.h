#pragma once
#include <windows.h>
#include "pch.h"
#include "System/TimeSystem.h"
#include "System/InputSystem.h"
#include "Renderer/IRenderer.h"

#define MAX_LOADSTRING 100

using Microsoft::WRL::ComPtr;

class GameApp : InputProcesser
{
public:
	GameApp(HINSTANCE hInstance);
	virtual ~GameApp();

	static HWND		hwnd;		//자주필요하니 포인터 간접접근을 피하기위해 정적멤버로 만들었다.
	static GameApp* InstancePtr;			// 생성자에서 인스턴스 포인터를 보관한다.

public:
	HACCEL			hAccelTable;
	MSG				msg;
	HINSTANCE		hInstance;                         // 현재 인스턴스입니다.
	WCHAR			titleName[MAX_LOADSTRING];         // 제목 표시줄 텍스트입니다.
	WCHAR			windowClassName[MAX_LOADSTRING];   // 기본 창 클래스 이름입니다.
	WNDCLASSEXW		wcex;								
	int				cmdShowCount;
	UINT			clientWidth;						// 창 넓이
	UINT			clientHeight;						// 창 높이

	GameTimer		gameTimer;
	InputSystem		inputSystem;

	std::shared_ptr<IRenderer>	renderer{};

	UINT internalWidth = 1920;
	UINT internalHeight = 1080;
	bool isResize = false;
public:
	// 윈도우 정보 등록,생성,보이기 한다.
	bool Initialize(UINT Width, UINT Height);
	virtual bool OnInitialize();

	virtual bool Run();

	void Update();
	void Render();
	virtual void OnUpdate();		// Update할 내용 정의
	virtual void OnRender() = 0;	// Render할 내용 정의

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker);
};