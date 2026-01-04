#pragma once
#include "pch.h"
#include "GameApp.h"
#include "System/SceneSystem.h"
#include "Renderer/ImguiRenderer.h"
#include "Renderer/RenderQueue.h"
#include "Editor.h"
#include <map>
#include "Renderer/DirectX11Renderer.h"

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
	std::unique_ptr<SceneSystem> sceneSystem{};
	std::unique_ptr<Editor> editor{};
	
	vector<std::shared_ptr<IRenderPass>> renderPasses;
	std::shared_ptr<GameObject> freeCamera{}; // 에디터 모드에서 보는 카메라

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker) override;

private:
	void BeginRender();
	void EndRender();

	std::shared_ptr<DirectX11Renderer> dxRenderer{};
};