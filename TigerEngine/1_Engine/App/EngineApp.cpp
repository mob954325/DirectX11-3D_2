#include "EngineApp.h"

EngineApp::EngineApp(HINSTANCE hInstance)
	: GameApp(hInstance)
{
}

EngineApp::~EngineApp()
{
}

bool EngineApp::OnInitialize()
{
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
	EndRender();
}

void EngineApp::BeginRender()
{
	renderer->BeginRender();
}

void EngineApp::EndRender()
{
	renderer->EndRender();
}