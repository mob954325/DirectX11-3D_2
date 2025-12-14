#pragma once
#include "pch.h"
#include "GameApp.h"
#include "Scene/Scene.h"
#include "Renderer/ImguiRenderer.h"
#include <map>

/// <summary>
/// 렌더 파이프라인이 흐름을 관리하는 앱
/// </summary>
class EngineApp : public GameApp
{
public:
	EngineApp(HINSTANCE hInstance);
	~EngineApp();

	bool OnInitialize() override;
	void OnUpdate() override;
	void OnRender() override;

	std::unique_ptr<ImguiRenderer> imguiRenderer{};
	Scene scene; // TODO : 씬 교체할 수 있게 수정하기

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:
	void BeginRender();
	void EndRender();
};