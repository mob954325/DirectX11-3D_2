#include "GameApp.h"
#include "pch.h"
#include "Helper.h"

// Debug 모듈관련 헤더 파일 및 라이브러리
// #include "Helper.h"
// #include <dbghelp.h>
// #include <minidumpapiset.h>
//  
// #pragma comment(lib, "Dbghelp.lib")

GameApp* GameApp::InstancePtr = nullptr;
HWND GameApp::hwnd;

#define USE_FLIPMODE 1

LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return  GameApp::InstancePtr->WndProc(hWnd, message, wParam, lParam);
}

void CreateDump(EXCEPTION_POINTERS* pExceptionPointers)
{
	wchar_t moduleFileName[MAX_PATH] = { 0, };
	std::wstring fileName(moduleFileName);
	if (GetModuleFileName(NULL, moduleFileName, MAX_PATH) == 0) {
		fileName = L"unknown_project.dmp"; // 예외 상황 처리
	}
	else
	{
		fileName = std::wstring(moduleFileName);
		size_t pos = fileName.find_last_of(L"\\/");
		if (pos != std::wstring::npos) {
			fileName = fileName.substr(pos + 1); // 파일 이름 추출
		}

		pos = fileName.find_last_of(L'.');
		if (pos != std::wstring::npos) {
			fileName = fileName.substr(0, pos); // 확장자 제거
		}
		fileName += L".dmp";
	}

	HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return;

	CloseHandle(hFile);
}

LONG WINAPI CustomExceptionHandler(EXCEPTION_POINTERS* pExceptionPointers)
{
	int msgResult = MessageBox(
		NULL,
		L"Should Create Dump ?",
		L"Exception",
		MB_YESNO | MB_ICONQUESTION
	);

	if (msgResult == IDYES) {
		CreateDump(pExceptionPointers);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

GameApp::GameApp(HINSTANCE hInstance)
	:hInstance(hInstance), windowClassName(L"DefaultWindowCalss"), titleName(L"GameApp"), clientWidth(1024), clientHeight(768)
{
	GameApp::InstancePtr = this;
	wcex.hInstance = hInstance;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = DefaultWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = windowClassName;
}

GameApp::~GameApp()
{

}

void GameApp::CreatDeviceAndContext()
{
	// 1. D3D11 Device, DeviceContext 생성

	// https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/ne-d3d11-d3d11_create_device_flag
	UINT creationFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
	/// https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/overviews-direct3d-11-devices-layers
	/// 디버그 계층을 지원하는 디바이스를 만들도록 플래그 추가
	/// 
	/// 디버그 계층은 광범위한 추가 매개 변수 및 일관성 유효성 검사(예: 셰이더 링크 및 리소스 바인딩 유효성 검사, 매개 변수 일관성 유효성 검사 및 오류 설명 보고)를 제공합니다.
	/// 해당 설정을 사용할 경우 애플리케이션이 상당히 느려진다.
	creationFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif //  _DEBUG

	// 그래픽 카드 하드웨어의 스펙으로 호환되는 가장 높은 DirectX 기능레벨로 생성하여 드라이버가 작동한다.
	// 인터페이스는 Direct3D11이지만 GPU 드라이버는 D3D12 드라이버가 작동될 수 있다.
	D3D_FEATURE_LEVEL featureLevels[] =
	{	// 0번 index부터 순서대로 시도
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0,D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL actualFeatureLevel;	// 최종 feature level 저장 변수

	HR_T(D3D11CreateDevice
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		creationFlag,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&device,
		&actualFeatureLevel,
		&deviceContext
	));
}

void GameApp::CreateSwapChain()
{
	// 2. 스왑체인 생성을 위한 DXGI Factory 생성
	UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

	ComPtr<IDXGIFactory2> pFactory;
	HR_T(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pFactory)));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
#if USE_FLIPMODE == 1
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
#else
	swapChainDesc.BufferCount = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
#endif
	swapChainDesc.Width = clientWidth;
	swapChainDesc.Height = clientHeight;

	// 하나의 픽셀이 채널 RGBA 각 8비트 형식으로 표현
	// Unsigned Normalized Integer 8비트 정수(0~255)단계를 부동소수점으로 정규화한 0.0~1.0으로 매핑하여 표현한다.
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 스왑 체인의 백 버퍼가 렌더링 파이프라인의 최종 출력 대상으로 사용
	swapChainDesc.SampleDesc.Count = 1;	// 멀티 샘플링 사용 안함
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE; // 투명도 조작 무시 | recommand for flip mode ?
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // 전체 화면 전환을 허용
	swapChainDesc.Scaling = DXGI_SCALING_NONE; // 창의 크기와 백 버퍼의 크기가 다를 때. 백버퍼 크기에 맞게 스케일링 하지 않는다.

	HR_T(pFactory->CreateSwapChainForHwnd
	(
		device.Get(),
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	// 3. 랜더타겟 뷰 생성. 랜더타겟 뷰는 "여기에 그림을 그려라"라고 GPU에게 알려주는 역할을 하는 객체
	// 텍스쳐와 영구적으로 연결되는 객체
	ComPtr<ID3D11Texture2D> pBackBufferTexture;
	HR_T(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(device->CreateRenderTargetView(pBackBufferTexture.Get(), nullptr, renderTargetView.GetAddressOf()));
}

void GameApp::ResizeSwapChain()
{
	// SwapChain 리사이즈
	UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

	ComPtr<IDXGIFactory2> pFactory;
	HR_T(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pFactory)));

	swapChain->ResizeBuffers(0, clientWidth, clientHeight, DXGI_FORMAT_B8G8R8A8_UNORM, dxgiFactoryFlags);
	
	// RTV 만들기
	//renderTargetView.Reset();
	ComPtr<ID3D11Texture2D> pBackBufferTexture;
	HR_T(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(device->CreateRenderTargetView(pBackBufferTexture.Get(), nullptr, renderTargetView.GetAddressOf()));
}

bool GameApp::Initialize(UINT Width, UINT Height)
{
	SetUnhandledExceptionFilter(CustomExceptionHandler);

	clientWidth = Width;
	clientHeight = Height;

	// 등록
	RegisterClassExW(&wcex);

	// 원하는 크기가 조정되어 리턴
	RECT rcClient = { 0, 0, (LONG)Width, (LONG)Height };
	AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

	//생성
	hwnd = CreateWindowW(windowClassName, titleName, WS_OVERLAPPEDWINDOW,
		100, 100,	// 시작 위치
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		nullptr, nullptr, hInstance, nullptr);

	if (!hwnd)
	{
		return false;
	}

	// 윈도우 보이기
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	// 시스템 초기화
	inputSystem.Initialize(hwnd, this);
	CreatDeviceAndContext();
	CreateSwapChain();

	// App 초기화
	if (!OnInitialize()) return false;

	// 타이머 초기화
	gameTimer.Reset();

	return true;
}

bool GameApp::OnInitialize()
{
	return true;
}

bool GameApp::Run()
{
	// PeekMessage 메세지가 있으면 true,없으면 false
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			//윈도우 메시지 처리 
			TranslateMessage(&msg); // 키입력관련 메시지 변환  WM_KEYDOWN -> WM_CHAR
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
		}
	}
	return 0;
}

void GameApp::Update()
{
	gameTimer.Tick();
	inputSystem.Update(gameTimer.DeltaTime());
	freeCamera.Update(gameTimer.DeltaTime());
	OnUpdate();
}

void GameApp::Render()
{
	OnRender();

	float windowAspect = (float)clientWidth / (float)clientHeight;
	float targetAspect = (float)internalWidth / (float)internalHeight;

	if (windowAspect > targetAspect)
	{
		// Pillarbox → 좌우 여백
		scaledWidth = clientHeight * targetAspect;
		scaledHeight = clientHeight;
		offsetX = (clientWidth - scaledWidth) * 0.5f;
		offsetY = 0;
	}
	else
	{
		// Letterbox → 상하 여백
		scaledWidth = clientWidth;
		scaledHeight = clientWidth / targetAspect;
		offsetX = 0;
		offsetY = (clientHeight - scaledHeight) * 0.5f;
	}


	swapChain->Present(0, 0);
}

void GameApp::OnUpdate()
{

}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK GameApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED)
			break; // 최소화는 무시

		UINT width = LOWORD(lParam); // 새 너비
		UINT height = HIWORD(lParam); // 새 높이			
		if (clientWidth != width || clientHeight != height)
		{
			clientWidth = width;
			clientHeight = height;
			isResize = true;
		}
		break;
	}
	case WM_EXITSIZEMOVE:
		if (isResize)
		{
			ResizeSwapChain();
			isResize = false;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

void GameApp::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
	freeCamera.OnInputProcess(KeyState, KeyTracker, MouseState, MouseTracker);
}