#include "EngineApp.h"
#include "Renderer/DirectX11Renderer.h"

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

	return true;
}

void EngineApp::OnUpdate()
{
	// Scene의 오브젝트 업데이트 호출
}

void EngineApp::OnRender()
{
	// RenderPass들 호출

	BeginRender();
	// RenderSomething ...
	
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