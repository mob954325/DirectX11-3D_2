#include "EngineApp.h"
#include "imgui_impl_win32.h" // ImGui_ImplWin32_WndProcHandler 사용하기 위함
#include "../Manager/FBXResourceManager.h"
#include "../Manager/ShaderManager.h"
#include "../Manager/WorldManager.h"
#include "Entity/GameObject.h"
#include "../RenderPass/DirectionalLightPass.h"
#include "../RenderPass/GBufferRenderPass.h"
#include "../RenderPass/SkyboxRenderPass.h"
#include "../RenderPass/ShadowRenderPass.h"
#include "../RenderPass/DebugDrawPass.h"
#include "System/CameraSystem.h"

#include "Entity/Object.h"
#include "System/ObjectSystem.h"

EngineApp::EngineApp(HINSTANCE hInstance)
	: GameApp(hInstance)
{
}

EngineApp::~EngineApp()
{
}

bool EngineApp::OnInitialize()
{
	RegisterAllComponents();

	/* ------------------------------ init renderer ----------------------------- */
	dxRenderer = std::static_pointer_cast<DirectX11Renderer>(renderer); 
	imguiRenderer = std::make_unique<ImguiRenderer>();
	imguiRenderer->Initialize(hwnd, dxRenderer->GetDevice(), dxRenderer->GetDeviceContext());

	/* ------------------------------- init system ------------------------------ */
	FBXResourceManager::Instance().GetDevice(dxRenderer->GetDevice(), dxRenderer->GetDeviceContext());
	ShaderManager::Instance().CreateCB(dxRenderer->GetDevice());

	editor = std::make_unique<Editor>();
	editor->GetScreenSize(clientWidth, clientHeight);
	editor->Initialize(dxRenderer->GetDevice(), dxRenderer->GetDeviceContext());
	editor->GetDSV(dxRenderer->GetDepthStencilView());
	editor->GetRTV(dxRenderer->GetBackBufferRTV());

	SceneSystem::Instance().AddScene();				// create first scene
	SceneSystem::Instance().SetCurrentSceneByIndex(); 	// render first scene

	/* --------------------------- create free camera --------------------------- */
	CameraSystem::Instance().CreateFreeCamera(clientWidth, clientHeight, SceneSystem::Instance().GetCurrentScene().get());
	WorldManager::Instance().CreateDirectionalLightFrustum(); // create directional light 


	/* ----------------------------- init renderpass ---------------------------- */
	// NOTE : 랜더링하는 순서대로 추가 할 것
	auto shadowPass = std::make_shared<ShadowRenderPass>();
	shadowPass->Init(dxRenderer->GetDevice(), dxRenderer->GetDeviceContext(), CameraSystem::Instance().GetFreeCamera());
	renderPasses.push_back(shadowPass);

	WorldManager::Instance().shaderResourceView = shadowPass->GetShadowSRV();

	auto gpass = std::make_shared<GBufferRenderPass>();
	gpass->Init(dxRenderer->GetDevice(), dxRenderer->GetDeviceContext(), clientWidth, clientHeight);
	gpass->SetDepthStencilView(dxRenderer->GetDepthStencilView());
	renderPasses.push_back(gpass);

	auto dlpass = std::make_shared<DirectionalLightPass>();
	dlpass->Init(dxRenderer->GetDevice());
	dlpass->SetGBufferSRV(gpass->GetShaderResourceViews());
	dlpass->SetDepthStencilView(dxRenderer->GetDepthStencilView());
	dlpass->SetRenderTargetView(dxRenderer->GetBackBufferRTV());
	dlpass->SetShadowSRV(shadowPass->GetShadowSRV());
	renderPasses.push_back(dlpass);

	auto sbpass = std::make_shared<SkyboxRenderPass>();
	sbpass->Init(dxRenderer->GetDevice(), dxRenderer->GetDeviceContext(), clientWidth, clientHeight);
	sbpass->SetDepthStencilView(dxRenderer->GetDepthStencilView());
	sbpass->SetRenderTargetView(dxRenderer->GetBackBufferRTV());
	renderPasses.push_back(sbpass);

	return true;
}

void EngineApp::OnUpdate()
{
	SceneSystem::Instance().BeforUpdate();
	
	// Scene의 오브젝트 업데이트 호출
	SceneSystem::Instance().UpdateScene(GameTimer::Instance().DeltaTime());

	CameraSystem::Instance().FreeCameraUpdate(GameTimer::Instance().DeltaTime());

	WorldManager::Instance().Update();

	editor->Update();
}

void EngineApp::OnRender()
{
	BeginRender(); 					// 업데이트 준비
	
	auto freeCam = CameraSystem::Instance().GetFreeCamera();	

	for(auto& pass : renderPasses)
	{	
		if(typeid(*pass) == typeid(GBufferRenderPass))
		{
			dxRenderer->ProcessScene(SceneSystem::Instance().GetCurrentScene(), pass, freeCam);
		}
		else if(typeid(*pass) == typeid(ShadowRenderPass))
		{
			dxRenderer->ProcessScene(SceneSystem::Instance().GetCurrentScene(), pass, freeCam); 
		}
		else
		{
			dxRenderer->ProcessScene(nullptr, pass, freeCam);  // 렌더러가 씬을 렌더링
		}
	}

	editor->Render(hwnd); 	// 엔진 오버레이 렌더링
	imguiRenderer->Render();		// imgui 렌더링

	editor->RenderEnd(dxRenderer->GetDeviceContext());
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
	editor->OnInputProcess(KeyState, KeyTracker, MouseState, MouseTracker);
	InputSystem::Instance().UpdateRegisterInput(KeyState,KeyTracker, MouseState, MouseTracker);
}

// ================= 컴포넌트 등록 =================

#include "../Components/FBXData.h"
#include "../Components/FBXRenderer.h"
#include "Entity/Transform.h"
#include "Entity/Camera.h"
#include "System/ComponentFactory.h"

void EngineApp::RegisterAllComponents()
{
	ComponentFactory::Instance().Register<FBXData>("FBXData");
	ComponentFactory::Instance().Register<FBXRenderer>("FBXRenderer");
	ComponentFactory::Instance().Register<Transform>("Transform");
	ComponentFactory::Instance().Register<Transform>("Camera");
}