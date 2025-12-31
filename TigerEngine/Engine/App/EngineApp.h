#pragma once
#include "pch.h"
#include "GameApp.h"
#include "System/SceneSystem.h"
#include "Renderer/ImguiRenderer.h"
#include "Renderer/RenderQueue.h"
#include "Editor.h"
#include <map>
#include "RenderPass/BasicRenderPass.h"
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
	
	std::shared_ptr<BasicRenderPass> basicRenderPass{};

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:
	void BeginRender();
	void EndRender();

	std::shared_ptr<DirectX11Renderer> dxRenderer{};
	std::unique_ptr<RenderQueue> renderQueue{};
};