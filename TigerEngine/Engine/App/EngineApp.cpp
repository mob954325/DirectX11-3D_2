#include "EngineApp.h"
#include "imgui_impl_win32.h" // ImGui_ImplWin32_WndProcHandler 사용하기 위함
#include "Manager/FBXResourceManager.h"
#include "Manager/ShaderManager.h"
#include <Entity/GameObject.h>
#include <Entity/Camera.h>

EngineApp::EngineApp(HINSTANCE hInstance)
	: GameApp(hInstance)
{
}

EngineApp::~EngineApp()
{
}

bool EngineApp::OnInitialize()
{
	/* ------------------------------ init renderer ----------------------------- */
	// TODO Directx11renderer에만 processScene이 있음 구조 수정할 것
	dxRenderer = std::static_pointer_cast<DirectX11Renderer>(renderer); 
	imguiRenderer = std::make_unique<ImguiRenderer>();
	imguiRenderer->Initialize(hwnd, dxRenderer->GetDevice(), dxRenderer->GetDeviceContext());

	/* ------------------------------- init system ------------------------------ */
	FBXResourceManager::Instance().GetDevice(dxRenderer->GetDevice(), dxRenderer->GetDeviceContext());
	ShaderManager::Instance().CreateCB(dxRenderer->GetDevice());

	sceneSystem = std::make_unique<SceneSystem>();
	editor = std::make_unique<Editor>();

	sceneSystem->AddScene();				// create first scene
	sceneSystem->SetCurrentSceneByIndex(); 	// render first scene

	/* ----------------------------- init renderpass ---------------------------- */
	basicRenderPass = std::make_shared<BasicRenderPass>();
	basicRenderPass->Init(dxRenderer->GetDevice());

	/* ------------------------------ init freeCam ------------------------------ */
	freeCamera = std::make_shared<GameObject>();
	freeCamera->SetName("Main Camera");
	freeCamera->SetScene(sceneSystem->GetCurrentScene().get());
	freeCamera->GetTransform()->position = { 0, 0, -30};
	sceneSystem->GetCurrentScene()->AddGameObject(freeCamera); // scene에 카메라 등록

	auto camComp = freeCamera->AddComponent<Camera>();
	camComp->SetProjection(DirectX::XM_PIDIV2, clientWidth, clientHeight, 0.1, 1000);

	return true;
}

void EngineApp::OnUpdate()
{
	sceneSystem->BeforUpdate();
	
	// Scene의 오브젝트 업데이트 호출
	sceneSystem->UpdateScene(GameTimer::Instance().DeltaTime());
}

void EngineApp::OnRender()
{
	BeginRender(); 					// 업데이트 준비

	auto rp = std::dynamic_pointer_cast<IRenderPass>(basicRenderPass); // 임시
	dxRenderer->ProcessScene(sceneSystem->GetCurrentScene(), rp, freeCamera->GetComponent<Camera>());  // 렌더러가 씬을 렌더링

	editor->Render(sceneSystem); 	// 엔진 오버레이 렌더링
	imguiRenderer->Render();		// imgui 렌더링

	EndRender(); 					// 업데이트 마무리
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

void EngineApp::OnInputProcess(const Keyboard::State &KeyState, const Keyboard::KeyboardStateTracker &KeyTracker, const Mouse::State &MouseState, const Mouse::ButtonStateTracker &MouseTracker)
{
	__super::OnInputProcess(KeyState, KeyTracker, MouseState, MouseTracker);
	InputSystem::Instance().UpdateRegisterInput(KeyState,KeyTracker, MouseState, MouseTracker);
}