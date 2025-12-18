#include "EngineApp.h"
#include "Renderer/DirectX11Renderer.h"
#include "imgui_impl_win32.h" // ImGui_ImplWin32_WndProcHandler 사용하기 위함

EngineApp::EngineApp(HINSTANCE hInstance)
	: GameApp(hInstance)
{
}

EngineApp::~EngineApp()
{
}

bool EngineApp::OnInitialize()
{
	std::shared_ptr<DirectX11Renderer> dxRenderer = std::dynamic_pointer_cast<DirectX11Renderer>(renderer);
	imguiRenderer = std::make_unique<ImguiRenderer>();
	imguiRenderer->Initialize(hwnd, dxRenderer->GetDevice(), dxRenderer->GetDeviceContext());

	sceneSystem = std::make_unique<SceneSystem>();
	renderQueue = std::make_unique<RenderQueue>();

	return true;
}

void EngineApp::OnUpdate()
{
	// Scene의 오브젝트 업데이트 호출
	sceneSystem->UpdateScene(gameTimer.DeltaTime());
}

void EngineApp::OnRender()
{
	renderQueue->Clear();

	// RenderPass들 호출	
	sceneSystem->RenderScene(renderQueue);

	BeginRender();	
	std::shared_ptr<DirectX11Renderer> dxRenderer = 
        std::dynamic_pointer_cast<DirectX11Renderer>(renderer);
    ComPtr<ID3D11DeviceContext> context = dxRenderer->GetDeviceContext();

	for (const auto& command : renderQueue->GetCommand())
    {
        command->Execute(context);
    }

	imguiRenderer->Render();	
	EndRender();
}

void EngineApp::BeginRender()
{
	imguiRenderer->BeginRender();
	renderer->BeginRender();
}

void EngineApp::EndRender()
{
	renderer->EndRender();
	imguiRenderer->EndRender();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT EngineApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}