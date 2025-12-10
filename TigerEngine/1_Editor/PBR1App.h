#pragma once
#include "SkeletalModel.h"

// DirectX11 
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

// DirectXTK
#include <directxtk/SimpleMath.h>
#include <directxtk/CommonStates.h> // https://github.com/microsoft/DirectXTK/wiki/CommonStates
#include <directxtk/Effects.h>		// https://github.com/microsoft/DirectXTK/wiki/BasicEffect

// imgui
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui.h"

// other files
#include "../Common/GameApp.h"
#include "DebugDraw.h"
#include <deque>

using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

class PBR1App : public GameApp
{
public:
	PBR1App(HINSTANCE hInstance);
	~PBR1App();

	// Model
	deque<unique_ptr<SkeletalModel>> objects;

	// 렌더링 파이프라인을 구성하는 필수 객체 인터페이스
	ComPtr<ID3D11PixelShader>		appPS{};				// 사용할 픽셀 셰이더
	ComPtr<ID3D11DepthStencilView>	depthStencilView{};	// 깊이 값 처리를 위한 뎊스스텐실 뷰
	ComPtr<ID3D11BlendState>		blendState{};				// 혼합 상태 객체 
	ComPtr<ID3D11DepthStencilState> depthStencilStateAllMask{};
	ComPtr<ID3D11DepthStencilState> depthStencilStateZeroMask{};
	ComPtr<ID3D11RasterizerState>	rasterizerState{};
	ComPtr<ID3D11RasterizerState>	transparentRasterizerState{};

	// vertex shaderes
	ComPtr<ID3D11VertexShader> skinnedMeshVertexShader{};


	// 렌더링 파이프라인에 적용하는 객체와 정보
	ComPtr<ID3D11InputLayout> inputLayout{};			// 입력 레이아웃
	ComPtr<ID3D11Buffer> constantBuffer{};			// 상수 버퍼
	ComPtr<ID3D11Buffer> materialBuffer{};					// 재질 버퍼

	// 리소스 객체
	ComPtr<ID3D11SamplerState> samplerLinear;	// 샘플링 객체

	// 좌표계 변환을 위한 행렬 모음
	Matrix world;				// 월드 좌표계 공간으로 변환을 위한 행렬, origin 위치에 있는 큐브 행렬
	Matrix view;				// 뷰 좌표계 공간으로 변환을 위한 행렬.
	Matrix projection;		// 단위 장치 좌표계 ( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.

	Vector4 directLightDirection;				// Directional Light의 방향
	Color directLightColor{ 1,1,1,1 };			// Directional Light의 색

	// 빛
	Vector4 m_LightDirectionInitial{ 0.0f, -1.0f, 1.0f, 1.0f };
	Vector4 m_LightAmbient{ 0.1f, 0.1f, 0.1f, 0.1f }; // 환경광 반사 계수
	Vector4 m_LightDiffuse{ 0.9f, 0.9f, 0.9f, 1.0f }; // 난반사 계수
	Vector4 m_LightSpecular{ 0.9f, 0.9f, 0.9f, 1.0f }; // 정반사 계수
	FLOAT m_Shininess = 40.0f; // 광택 지수

	float m_Near = 0.01f;
	float m_Far = 3000.0f;
	float m_PovAngle = XM_PIDIV2;

	// 그림자
	ComPtr<ID3D11VertexShader> shadowMapVS;			//  shadowMap에 사용할 vs
	ComPtr<ID3D11PixelShader> shadowMapPS{};

	ComPtr<ID3D11Texture2D> shadowMap;				// 깊이 값을 기록할 텍스처
	ComPtr<ID3D11DepthStencilView> shadowMapDSV;		// 깊이 값 기록을 설정할 객체
	ComPtr<ID3D11ShaderResourceView> shadowMapSRV;	// 깊이 버퍼를 슬롯에서 설정하고 사용하기 위한 객체

	Matrix shadowView{};
	Matrix shadowProjection{};
	Vector3 shadowLookAt{};
	Vector3 shadowMapPosition{};
	float shadowFrustumAngle = XM_PIDIV4;

	Viewport shadowViewport = { 0, 0, 8192, 8192, 0.0f, 1.0f }; // x, y, width, height, min, max
	D3D11_VIEWPORT renderViewport = {};
	float shadowForwardDistFromCamera = 1.0f;
	float shadowNear = 400.0f;
	float shadowFar = 3000.0f;
	const float shadowMinNear = 400.0f;
	const float shadowMinFar = 1001.0f;

	// 그림자 디버그
	ComPtr<ID3D11InputLayout> debugDrawInputLayout{}; // debug inputlayout
	unique_ptr<DirectX::CommonStates> debugStates;	// CommonState : provide the stock? state objects
	unique_ptr<PrimitiveBatch<VertexPositionColor>> debugBatchColor; // vertexPositionColor : proivde input layout
	unique_ptr<DirectX::BasicEffect> debugEffect;	// BasicEffect : provide the vertex and pixel shader progrmas

	Vector3 shadowUpDistFromLookAt{ 0, 1000, 0 };
	Vector3 groundScale{ 1000, 1, 1000 };

	// model buffer
	ComPtr<ID3D11Buffer> transformBuffer{};
	ComPtr<ID3D11Buffer> bonePoseBuffer{};
	ComPtr<ID3D11Buffer> boneOffsetBuffer{};

	// dxgi들
	ComPtr<IDXGIDevice3> dxgiDevice3{};
	ComPtr<IDXGIFactory6> dxgiFactory6{};
	ComPtr<IDXGIAdapter1> dxgiAdapter1{};
	ComPtr<IDXGIAdapter3> dxgiAdapter3{};

	// 스카이 박스
	ComPtr<ID3D11ShaderResourceView> skyboxTexture;

	ComPtr<ID3D11VertexShader> skyboxVS{};				// 스카이 박스용 정점 셰이더
	ComPtr<ID3D11PixelShader> skyboxPS{};				// 스카이 박스용 픽셀 셰이더
	ComPtr<ID3D11InputLayout> skyboxInputLayout{};		// 입력 레이아웃
	ComPtr<ID3D11Buffer> skyboxVertexBuffer{};			// 스카이 박스 정점 버퍼
	UINT skyboxVertexBufferStride = 0;							// 스카이 박스 정점 하나의 버퍼 크기
	UINT skyboxVertexBufferOffset = 0;							// 스카이 박스 정점 버퍼의 오프셋
	ComPtr<ID3D11Buffer> skyboxIndexBuffer;						// 스카이 박스가 사용할 인덱스 버퍼
	int skyboxIndices = 0;										// 스카이박스 인덱스 버퍼 개수

	ComPtr<ID3D11RasterizerState> skyRasterizerState{};	// 스카이박스 래스터라이저 상태
	ComPtr<ID3D11DepthStencilState> skyDepthStencilState{};	// 스카이 박스를 위한 뎊스스텐실 상태 개체

	// PBR
	ComPtr<ID3D11PixelShader> PbrPS{};
	float roughness = 0;
	float metalness = 0;

	bool useBaseColor = true; // NOTE: 텍스처 없는 오브젝트는 강제로 플래그 활성화되서 출력이 안될 수 있음.
	bool useNormal = true;
	bool useMetalness = true;
	bool useRoughness = true;

	// PBR IBL
	ComPtr<ID3D11ShaderResourceView> IBLIrradianceResource;		// cubeMap
	ComPtr<ID3D11ShaderResourceView> IBLSpecularResource;		// cubeMap
	ComPtr<ID3D11ShaderResourceView> IBLLookUpTableResource;	// Texture2D

	// =============================================================
	virtual bool OnInitialize();
	virtual void OnUpdate();
	virtual void OnRender();

	void DepthOnlyPass();
	void RenderPass();
	void RenderSkyBox();

	bool InitImGUI();
	void RenderImGUI();
	void UninitImGUI();

	bool InitD3D();
	bool InitScene();
	bool InitEffect();
	void InitDebugDraw();	// 디버그 관련 초기화 함수
	void InitShdowMap();	// ShadowMap 관련 초기화 함수
	bool InitDxgi();		// 메모리 사용량 디버그용 dxgi
	bool InitSkyBox();

	void DrawFrustum(Matrix worldMat, Matrix viewMat, Matrix proejctionMat,
		float angle, float AspectRatio, float nearZ, float farZ, XMVECTORF32 color = Colors::Red); // 절두체 그리는 함수

	void ShowMatrix(const DirectX::XMFLOAT4X4& mat, const char* label = "Matrix");

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker) override;
};