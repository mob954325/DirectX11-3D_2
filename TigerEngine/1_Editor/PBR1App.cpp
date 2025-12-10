#include "PBR1App.h"
#include "../Common/Helper.h"

#include <directxtk/SimpleMath.h>
#include <dxgidebug.h>
#include <dxgi1_3.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <stack>

#include <string>
#include <directxtk/DDSTextureLoader.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;
using Microsoft::WRL::ComPtr;

#define USE_FLIPMODE 1 // 경고 메세지를 없애려면 Flip 모드를 사용한다.

// 상수 버퍼
struct ConstantBuffer
{
	Matrix cameraView;
	Matrix cameraProjection;

	Vector4 lightDirection;
	Matrix shadowView;
	Matrix shadowProjection;

	Color lightColor;

	Vector4 ambient;	// 환경광
	Vector4 diffuse;	// 난반사
	Vector4 specular;	// 정반사
	FLOAT shininess;	// 광택지수
	Vector3 CameraPos;	// 카메라 위치

	FLOAT metalness;	//  
	FLOAT roughness;	//
	Vector2 pad3;
};

struct CubeVertex
{
	Vector3 position;
};

std::string WStringToUTF8(const std::wstring& wstr)
{
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
		(int)wstr.size(),
		nullptr, 0, nullptr, nullptr);

	std::string result(sizeNeeded, 0);

	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
		(int)wstr.size(),
		&result[0], sizeNeeded,
		nullptr, nullptr);

	return result;
}

PBR1App::PBR1App(HINSTANCE hInstance)
	: GameApp(hInstance)
{

}

PBR1App::~PBR1App()
{
	UninitImGUI();
}

void PBR1App::InitDebugDraw()
{
	debugStates = std::make_unique<CommonStates>(device.Get());
	debugBatchColor = std::make_unique<PrimitiveBatch<VertexPositionColor>>(deviceContext.Get());

	debugEffect = std::make_unique<BasicEffect>(device.Get());
	debugEffect->SetVertexColorEnabled(true);
	debugEffect->SetView(shadowView);
	debugEffect->SetProjection(shadowProjection);

	{
		void const* shaderByteCode;
		size_t byteCodeLength;

		debugEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength); // ??

		// https://github.com/Microsoft/DirectXTK/wiki/throwIfFailed -> 이거 문서에 있던건데 이거 결국 HRESULT 값을 반환한다는 소리임
		HR_T(device->CreateInputLayout(
			VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			debugDrawInputLayout.ReleaseAndGetAddressOf()));
	}
}

void PBR1App::InitShdowMap()
{
	// create shadow map texure desc
	D3D11_TEXTURE2D_DESC texDesc = {}; // https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/ns-d3d11-d3d11_texture2d_desc
	texDesc.Width = (UINT)shadowViewport.width;
	texDesc.Height = (UINT)shadowViewport.height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	HR_T(device->CreateTexture2D(&texDesc, nullptr, shadowMap.GetAddressOf()));

	// DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	HR_T(device->CreateDepthStencilView(shadowMap.Get(), &descDSV, shadowMapDSV.GetAddressOf()));

	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	HR_T(device->CreateShaderResourceView(shadowMap.Get(), &srvDesc, shadowMapSRV.GetAddressOf()));

	// 빛 계산 ( pov )
	shadowProjection = XMMatrixPerspectiveFovLH(shadowFrustumAngle, shadowViewport.width / (FLOAT)shadowViewport.height, shadowNear, shadowFar); // 그림자 절두체
	shadowLookAt = freeCamera.position + freeCamera.GetForward() * shadowForwardDistFromCamera;	// 바라보는 방향 = 카메라 위치 + 카메라 바라보는 방향으로부터 떨어진 태양의 위치
	shadowMapPosition = freeCamera.position + ((Vector3)-directLightDirection * shadowUpDistFromLookAt);	// 위치
	shadowView = XMMatrixLookAtLH(shadowMapPosition, shadowLookAt, Vector3(0.0f, 1.0f, 0.0f));
}

bool PBR1App::InitSkyBox()
{
	const float width = 1.0f;
	const float height = 1.0f;
	const float depth = 1.0f;
	CubeVertex skyboxVertices[] =
	{
		{ Vector3(-width, -height, -depth) },
		{ Vector3(-width, +height, -depth) },
		{ Vector3(+width, +height, -depth) },
		{ Vector3(+width, -height, -depth) },

		{ Vector3(-width, -height, +depth) },
		{ Vector3(+width, -height, +depth) },
		{ Vector3(+width, +height, +depth) },
		{ Vector3(-width, +height, +depth) },

		{ Vector3(-width, +height, -depth) },
		{ Vector3(-width, +height, +depth) },
		{ Vector3(+width, +height, +depth) },
		{ Vector3(+width, +height, -depth) },

		{ Vector3(-width, -height, -depth) },
		{ Vector3(+width, -height, -depth) },
		{ Vector3(+width, -height, +depth) },
		{ Vector3(-width, -height, +depth) },

		{ Vector3(-width, -height, +depth) },
		{ Vector3(-width, +height, +depth) },
		{ Vector3(-width, +height, -depth) },
		{ Vector3(-width, -height, -depth) },

		{ Vector3(+width, -height, -depth) },
		{ Vector3(+width, +height, -depth) },
		{ Vector3(+width, +height, +depth) },
		{ Vector3(+width, -height, +depth) }
	};

	// 버텍스 버퍼
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(CubeVertex) * ARRAYSIZE(skyboxVertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = skyboxVertices;
	HR_T(device->CreateBuffer(&bufferDesc, &vbData, skyboxVertexBuffer.GetAddressOf()));

	skyboxVertexBufferStride = sizeof(CubeVertex); 	// 버텍스 버퍼의 정보
	skyboxVertexBufferOffset = 0;

	// 파이프라인에 바인딩할 InputLayout 생성
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ComPtr<ID3DBlob> vertexShaderBuffer = nullptr;
	HR_T(CompileShaderFromFile(L"Shaders\\VS_Skybox.hlsl", "main", "vs_5_0", &vertexShaderBuffer));
	HR_T(device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), skyboxInputLayout.GetAddressOf()));

	// 파이프 라인에 바인딩할 정점 셰이더 생성
	HR_T(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, skyboxVS.GetAddressOf()));

	// 인덱스 버퍼
	WORD skyboxIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23,
	};

	bufferDesc.ByteWidth = sizeof(WORD) * ARRAYSIZE(skyboxIndices);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;

	skyboxVertexBufferStride = sizeof(CubeVertex); 	// 버텍스 버퍼의 정보
	skyboxVertexBufferOffset = 0;

	this->skyboxIndices = ARRAYSIZE(skyboxIndices); // 인덱스 개수 저장

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = skyboxIndices;
	HR_T(device->CreateBuffer(&bufferDesc, &ibData, skyboxIndexBuffer.GetAddressOf()));

	// 픽셀 셰이더
	ComPtr<ID3DBlob> sbPixelShaderBuffer = nullptr;
	HR_T(CompileShaderFromFile(L"Shaders\\PS_Skybox.hlsl", "main", "ps_5_0", &sbPixelShaderBuffer));
	HR_T(device->CreatePixelShader(sbPixelShaderBuffer->GetBufferPointer(), sbPixelShaderBuffer->GetBufferSize(), NULL, skyboxPS.GetAddressOf()));

	// 래스터라이저
	D3D11_RASTERIZER_DESC rasterizerState = {};
	rasterizerState.CullMode = D3D11_CULL_BACK;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.FrontCounterClockwise = true;

	HR_T(device->CreateRasterizerState(&rasterizerState, skyRasterizerState.ReleaseAndGetAddressOf()));

	// 스카이 박스 뎊스 스텐실 상태 개체 추가
	D3D11_DEPTH_STENCIL_DESC skyboxDsDesc;
	ZeroMemory(&skyboxDsDesc, sizeof(skyboxDsDesc));
	skyboxDsDesc.DepthEnable = false;
	skyboxDsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// 깊이 버퍼 사용 X
	// skyboxDsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;		
	skyboxDsDesc.StencilEnable = false;
	HR_T(device->CreateDepthStencilState(&skyboxDsDesc, skyDepthStencilState.GetAddressOf()));

	return true;
}

void PBR1App::DrawFrustum(Matrix worldMat, Matrix viewMat, Matrix proejctionMat,
	float angle, float AspectRatio, float nearZ, float farZ, XMVECTORF32 color)
{
	Vector3 scale = { 1,1,1 };

	// 절투체 만들기
	BoundingFrustum frustum{};
	BoundingFrustum::CreateFromMatrix(frustum, proejctionMat);

	Matrix frustumWorld = viewMat.Transpose();

	frustum.Transform(frustum, frustumWorld); // 위치 옮기기
	frustum.Transform(frustum, worldMat);

	debugEffect->SetWorld(Matrix::Identity);	// 해당 그림 위치 설정
	debugEffect->SetView(view);				// 해당 그림을 어디 기준으로 그릴지 설정
	debugEffect->SetProjection(projection);	// 해당 그림이 어디에 투영 될지 설정
	debugEffect->Apply(deviceContext.Get());

	// 문서에 따른 세팅 -> https://github.com/microsoft/DirectXTK/wiki/DebugDraw
	deviceContext->OMSetBlendState(debugStates->Opaque(), nullptr, 0xFFFFFFFF);
	deviceContext->OMSetDepthStencilState(debugStates->DepthNone(), 0);
	deviceContext->RSSetState(debugStates->CullNone());

	deviceContext->IASetInputLayout(debugDrawInputLayout.Get()); // 디버그용 InputLayout 적용

	debugBatchColor->Begin();
	DX::Draw(debugBatchColor.get(), frustum, color);

	debugBatchColor->End();
}

bool PBR1App::InitDxgi()
{
	// ============================================
	// 1. IDXGIDevice3 생성
	// ============================================
	HR_T(device->QueryInterface(__uuidof(IDXGIDevice3), reinterpret_cast<void**>(dxgiDevice3.GetAddressOf())));


	// ============================================
	// 1. DXGI 어댑터 생성
	// ============================================
	HR_T((CreateDXGIFactory1(__uuidof(IDXGIFactory6), reinterpret_cast<void**>(dxgiFactory6.GetAddressOf()))));

	HR_T(dxgiFactory6->EnumAdapters1(0, dxgiAdapter1.GetAddressOf()));

	// ============================================
	// 3. VRAM 사용량 (DXGI 1.4 - Windows 10+)
	// ============================================
	HR_T(dxgiAdapter1->QueryInterface(__uuidof(IDXGIAdapter3), reinterpret_cast<void**>(dxgiAdapter3.GetAddressOf())));

	return true;
}

bool PBR1App::OnInitialize()
{
	if (!InitD3D())
		return false;

	if (!InitScene())
		return false;

	if (!InitImGUI())
		return false;

	if (!InitEffect())
		return false;

	if (!InitDxgi())
		return false;

	if (!InitSkyBox())
		return false;

	InitShdowMap();
	InitDebugDraw();
	
	return true;
}

void PBR1App::OnUpdate()
{
	float delta = GameTimer::m_Instance->DeltaTime();

	Matrix scale = Matrix::Identity;
	Matrix rotate = Matrix::Identity;
	Matrix position = Matrix::Identity;

	// Camera
	freeCamera.GetCameraViewMatrix(view);

	for (auto& e : objects)
	{
		if (!e->isRemoved) e->Update();
	}
}

void PBR1App::OnRender()
{
	DepthOnlyPass();

#if USE_FLIPMODE == 1
	// Flip 모드에서는 매프레임 설정해야한다.
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get()); // depthStencilView 사용
#endif	
	// 화면 칠하기.
	Color color(0.1f, 0.2f, 0.3f, 1.0f);
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0); // 뎁스버퍼 1.0f로 초기화.

	// IBL 텍스처 리소스 넘겨주기
	deviceContext->PSSetShaderResources(8, 1, IBLIrradianceResource.GetAddressOf());		// Irradiance
	deviceContext->PSSetShaderResources(9, 1, IBLSpecularResource.GetAddressOf());		// Sepcular
	deviceContext->PSSetShaderResources(10, 1, IBLLookUpTableResource.GetAddressOf());	// LUT

	RenderSkyBox();
	RenderPass();

	// Debug Draw Test code ==============

	// 디버그를 위한 회전값 구하기
	Matrix shadowWorldMat = Matrix::CreateTranslation(shadowMapPosition);

	DrawFrustum(shadowWorldMat, shadowView, shadowProjection,
		shadowFrustumAngle,
		shadowViewport.width / (FLOAT)shadowViewport.height,
		shadowNear, shadowFar); // -> 제대로 출력 안됨

	// Debug Draw Test code END ==============

	// Render ImGui
	RenderImGUI();

	// 스왑체인 교체
	swapChain->Present(0, 0);
}

bool isplayed = false;

void PBR1App::DepthOnlyPass()
{
	// 바인딩 해제
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	deviceContext->VSSetShaderResources(4, 1, nullSRV);
	deviceContext->PSSetShaderResources(4, 1, nullSRV);

	//상수 버퍼 갱신
	ConstantBuffer cb;
	cb.cameraView = XMMatrixTranspose(view);
	cb.cameraProjection = XMMatrixTranspose(projection);
	cb.lightDirection = directLightDirection;
	cb.lightDirection.Normalize();
	cb.shadowView = XMMatrixTranspose(shadowView);
	cb.shadowProjection = XMMatrixTranspose(shadowProjection);
	cb.lightColor = directLightColor;

	cb.ambient = m_LightAmbient;
	cb.diffuse = m_LightDiffuse;
	cb.specular = m_LightSpecular;

	cb.shininess = m_Shininess;
	cb.CameraPos = freeCamera.position;

	// 뷰포트 설정 + DSV 초기화, RS, OM 설정
	D3D11_VIEWPORT viewport
	{
		shadowViewport.x, shadowViewport.y,
		shadowViewport.width, shadowViewport.height,
		shadowViewport.minDepth, shadowViewport.maxDepth
	};
	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->OMSetDepthStencilState(depthStencilStateAllMask.Get(), 1);
	deviceContext->OMSetRenderTargets(0, nullptr, shadowMapDSV.Get());
	deviceContext->ClearDepthStencilView(shadowMapDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// 렌더 파이프라인 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(inputLayout.Get());

	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	deviceContext->PSSetSamplers(0, 1, samplerLinear.GetAddressOf());

	deviceContext->VSSetShader(shadowMapVS.Get(), 0, 0);
	// m_pDeviceContext->PSSetShader(NULL, NULL, 0); // 렌더 타겟에 기록할 RGBA가 없으므로 실행하지 않는다.
	deviceContext->PSSetShader(shadowMapPS.Get(), NULL, 0); // 

	// 모델 draw 호출
	for (auto& e : objects)
	{
		if (!e->isRemoved) e->Draw(deviceContext, materialBuffer);
	}
}

void PBR1App::RenderPass()
{
	deviceContext->RSSetViewports(1, &renderViewport); // 뷰포트 되돌리기

	// Update Constant Values
	ConstantBuffer cb;
	cb.cameraView = XMMatrixTranspose(view);
	cb.cameraProjection = XMMatrixTranspose(projection);
	cb.lightDirection = directLightDirection;
	cb.lightDirection.Normalize();
	cb.lightColor = directLightColor;
	cb.shadowView = XMMatrixTranspose(shadowView);
	cb.shadowProjection = XMMatrixTranspose(shadowProjection);

	cb.ambient = m_LightAmbient;
	cb.diffuse = m_LightDiffuse;
	cb.specular = m_LightSpecular;

	cb.shininess = m_Shininess;
	cb.CameraPos = freeCamera.position;

	cb.roughness = roughness;
	cb.metalness = metalness;

	// 텍스처 및 샘플링 설정 
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(inputLayout.Get());

	deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	deviceContext->VSSetShader(skinnedMeshVertexShader.Get(), 0, 0);

	deviceContext->PSSetShader(PbrPS.Get(), 0, 0);

	deviceContext->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(1, 1, materialBuffer.GetAddressOf());

	deviceContext->PSSetSamplers(0, 1, samplerLinear.GetAddressOf());

	deviceContext->RSSetState(rasterizerState.Get());

	deviceContext->OMSetDepthStencilState(depthStencilStateAllMask.Get(), 1);
	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	deviceContext->PSSetShaderResources(4, 1, shadowMapSRV.GetAddressOf());

	// Draw 
	for (auto& e : objects)
	{
		e->Draw(deviceContext, materialBuffer);
	}
}

void PBR1App::RenderSkyBox()
{
	deviceContext->RSSetViewports(1, &renderViewport); // 뷰포트 되돌리기
	deviceContext->OMSetDepthStencilState(skyDepthStencilState.Get(), 1); // 뎊스 스텐실 설정

	// 카메라용 뷰 행렬과, 투영행렬
	Matrix m_skyboxProjection = XMMatrixPerspectiveFovLH(m_PovAngle, clientWidth / (FLOAT)clientHeight, 0.1, m_Far);

	ConstantBuffer cb;

	cb.cameraView = XMMatrixTranspose(view); // 쉐이더 코드 내부에서 이동 성분 제거함
	cb.cameraProjection = XMMatrixTranspose(m_skyboxProjection);

	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

	deviceContext->IASetInputLayout(skyboxInputLayout.Get());
	deviceContext->IASetIndexBuffer(skyboxIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, skyboxVertexBuffer.GetAddressOf(), &skyboxVertexBufferStride, &skyboxVertexBufferOffset);
	deviceContext->VSSetShader(skyboxVS.Get(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	deviceContext->RSSetState(skyRasterizerState.Get());
	deviceContext->PSSetShader(skyboxPS.Get(), nullptr, 0);
	deviceContext->PSSetShaderResources(5, 1, skyboxTexture.GetAddressOf());
	deviceContext->PSSetSamplers(0, 1, samplerLinear.GetAddressOf());

	deviceContext->DrawIndexed(skyboxIndices, 0, 0);
}

bool PBR1App::InitImGUI()
{
	bool isSetupSuccess = false;

	// Setup Dear ImGui context 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	isSetupSuccess = ImGui_ImplWin32_Init(hwnd);
	if (!isSetupSuccess) return false;

	isSetupSuccess = ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
	if (!isSetupSuccess) return false;

	return true;
}

void PBR1App::RenderImGUI()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(800, 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Once);	

	ImGui::Begin("ShadowMap");
	{
		ImTextureID img = (ImTextureID)(intptr_t)(shadowMapSRV.Get());
		ImGui::Image(img, ImVec2(256, 256));
		ImGui::DragFloat("shadowForwardDistFromCamera", &shadowForwardDistFromCamera);
		ImGui::DragFloat3("shadowUpDistFromLookAt", &shadowUpDistFromLookAt.x);

		ImGui::DragFloat("shadowNear", &shadowNear);
		ImGui::DragFloat("shadowFar", &shadowFar);

		if (shadowNear <= shadowMinNear) shadowNear = shadowMinNear;
		if (shadowFar <= shadowMinFar) shadowFar = shadowMinFar;

		ImGui::DragFloat("shadowFrustumAngle", &shadowFrustumAngle, 0.02f);

		// m_shadow 관련 갱신
		shadowProjection = XMMatrixPerspectiveFovLH(shadowFrustumAngle, shadowViewport.width / (FLOAT)shadowViewport.height, shadowNear, shadowFar);
		shadowLookAt = freeCamera.position + freeCamera.GetForward() * shadowForwardDistFromCamera;	// 바라보는 방향 = 카메라 위치 + 카메라 바라보는 방향으로부터 떨어진 태양의 위치
		shadowMapPosition = freeCamera.position + ((Vector3)-directLightDirection * shadowUpDistFromLookAt);	// 위치
		shadowView = XMMatrixLookAtLH(shadowMapPosition, shadowLookAt, Vector3(0.0f, 1.0f, 0.0f));
	}
	ImGui::End();

	ImGui::Begin("World Config");
	{
		// 카메라 위치 및 회전 설정
		ImGui::DragFloat3("Camera Position", &freeCamera.position.x);
		ImGui::DragFloat3("Camera Rotation", &freeCamera.rotation.x);

		if (ImGui::Button("Set Rotation"))
		{
			freeCamera.rotation.x = XMConvertToRadians(freeCamera.rotation.x);
			freeCamera.rotation.y = XMConvertToRadians(freeCamera.rotation.y);
			freeCamera.rotation.z = XMConvertToRadians(freeCamera.rotation.z);
		}


		// Near 값 설정
		ImGui::DragFloat("Near", &m_Near, 0.5f);

		// Far 값 설정
		ImGui::DragFloat("Far", &m_Far, 0.5f);

		// Fov 값 설정
		ImGui::DragFloat("Fov Angle", &m_PovAngle, 0.02f);

		if (m_Near <= 0.0f) m_Near = 0.01f;
		if (m_Far <= m_Near) m_Far = 0.2f;


		projection = XMMatrixPerspectiveFovLH(m_PovAngle, clientWidth / (FLOAT)clientHeight, m_Near, m_Far);

		ImGui::DragFloat3("Light Direction", &directLightDirection.x, 0.1f, -1.0f, 1.0f);

		ImGui::DragFloat("Roughness", &roughness, 0.01f, 0, 1);
		ImGui::DragFloat("Metalness", &metalness, 0.01f, 0, 1);
	}
	ImGui::End();

	ImGui::Begin("Object info");
	{
		if (!objects.empty())
		{
			stack<int> s;
			s.push(0);
			while (!s.empty())
			{
				int currBoneIndex = s.top(); s.pop();				
				
			}
		}
	}
	ImGui::End();

	// rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void PBR1App::UninitImGUI()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool PBR1App::InitD3D()
{
	// 4. viewport 설정
	renderViewport = {};
	renderViewport.TopLeftX = 0;
	renderViewport.TopLeftY = 0;
	renderViewport.Width = (float)clientWidth;
	renderViewport.Height = (float)clientHeight;
	renderViewport.MinDepth = 0.0f;
	renderViewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &renderViewport);

	// 5. 뎊스 스텐실 뷰 설정
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = clientWidth;
	descDepth.Height = clientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // https://learn.microsoft.com/ko-kr/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	// 뎊스 스탠실 상태 설정
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;                // 깊이 테스트 활성화
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 버퍼 업데이트 허용
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; // 작은 Z 값이 앞에 배치되도록 설정
	depthStencilDesc.StencilEnable = FALSE;            // 스텐실 테스트 비활성화

	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilStateZeroMask);

	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;                // 깊이 테스트 활성화
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 깊이 버퍼 업데이트 허용
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; // 작은 Z 값이 앞에 배치되도록 설정
	depthStencilDesc.StencilEnable = FALSE;            // 스텐실 테스트 비활성화

	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilStateAllMask);

	// create depthStencil texture
	ComPtr<ID3D11Texture2D> pTextureDepthStencil;
	HR_T(device->CreateTexture2D(&descDepth, nullptr, pTextureDepthStencil.GetAddressOf()));

	// create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // 사용되는 리소스 엑세스 방식 설정 : https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/ne-d3d11-d3d11_dsv_dimension 
	descDSV.Texture2D.MipSlice = 0;
	HR_T(device->CreateDepthStencilView(pTextureDepthStencil.Get(), &descDSV, depthStencilView.GetAddressOf()));

	// create blending state https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/ns-d3d11-d3d11_blend_desc
	// Color = SrcAlpha * SrcColor + (1 - SrcAlpha) * DestColor 
	// Alpha = SrcAlpha
	D3D11_BLEND_DESC descBlend = {};
	descBlend.RenderTarget[0].BlendEnable = true;						// blend 사용 여부

	// SrcBlend -> 소스 텍스처의 색상
	// DestBlend -> 이미 해당 자리에 그려져있는 색상
	descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			// D3D11_BLEND_SRC_ALPHA -> 픽셀 셰이더 결과 값의 알파 데이터 값
	descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	// D3D11_BLEND_INV_SRC_ALPHA -> D3D11_BLEND_SRC_ALPHA의 반전 값 ( 1 - 값 )
	descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				// SrcBlend 및 DestBlend 작업을 결합하는 방법을 정의, D3D11_BLEND_OP_ADD -> Add source 1 and source 2

	descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;			// D3D11_BLEND_ONE -> (1,1,1,1)
	descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;		// D3D11_BLEND_ZERO -> (0,0,0,0)
	descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;		// 

	descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;  // 모든 색 전부 사용

	device->CreateBlendState(&descBlend, blendState.GetAddressOf());

	// material buffer
	D3D11_BUFFER_DESC mbd = {};
	mbd.Usage = D3D11_USAGE_DEFAULT;
	mbd.ByteWidth = sizeof(Material);
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.CPUAccessFlags = 0;
	HR_T(device->CreateBuffer(&mbd, nullptr, materialBuffer.GetAddressOf()));

	return true;
}

bool PBR1App::InitScene()
{
	HRESULT hr = S_OK;

	// 6. Render() 에서 파이프라인에 바인딩할 상수 버퍼 생성
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(ConstantBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	HR_T(device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf()));

	// 쉐이더에 상수버퍼에 전달할 시스템 메모리 데이터 초기화
	world = XMMatrixIdentity();

	XMVECTOR Eye = XMVectorSet(0.0f, 10.0f, -8.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	view = XMMatrixLookAtLH(Eye, At, Up);
	projection = XMMatrixPerspectiveFovLH(m_PovAngle, clientWidth / (FLOAT)clientHeight, m_Near, m_Far);

	// 샘플링 상태 설정
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// 텍스처 샘플링할 때 사용할 필터링 방법
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// 범위 밖에 있는 텍스처 좌표 확인 방법
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;	// 샘플링된 데이터를 기존 데이터와 확인하는 방법
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR_T(device->CreateSamplerState(&sampDesc, samplerLinear.GetAddressOf()));

	// 래스터라이저
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FrontCounterClockwise = true;

	device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

	rasterizerDesc = {};
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FrontCounterClockwise = true;

	device->CreateRasterizerState(&rasterizerDesc, &transparentRasterizerState);

	// 모델들이 사용할 버퍼 만들기
	// 트랜스폼 상수 버퍼 만들기
	bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(TransformBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	HR_T(device->CreateBuffer(&bufferDesc, nullptr, transformBuffer.GetAddressOf()));

	// 본 포즈 버퍼 만들기
	bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(BonePoseBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	HR_T(device->CreateBuffer(&bufferDesc, nullptr, bonePoseBuffer.GetAddressOf()));

	// 본 오프셋 버퍼 만들기
	bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(BoneOffsetBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	HR_T(device->CreateBuffer(&bufferDesc, nullptr, boneOffsetBuffer.GetAddressOf()));

	HR_T(CreateDDSTextureFromFile(device.Get(), L"..\\Resource\\skyboxEnvHDR.dds", nullptr, skyboxTexture.GetAddressOf()));
	HR_T(CreateDDSTextureFromFile(device.Get(), L"..\\Resource\\skyboxDiffuseHDR.dds", nullptr, IBLIrradianceResource.GetAddressOf()));
	HR_T(CreateDDSTextureFromFile(device.Get(), L"..\\Resource\\skyboxSpecularHDR.dds", nullptr, IBLSpecularResource.GetAddressOf()));
	HR_T(CreateDDSTextureFromFile(device.Get(), L"..\\Resource\\skyboxBrdf.dds", nullptr, IBLLookUpTableResource.GetAddressOf()));

	return true;
}

bool PBR1App::InitEffect()
{
	// 2. 파이프라인에 바인딩할 InputLayout 생성
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ComPtr<ID3DBlob> vertexShaderBuffer = nullptr;
	HR_T(CompileShaderFromFile(L"Shaders\\VS_SkinnedMesh.hlsl", "main", "vs_5_0", vertexShaderBuffer.GetAddressOf()));
	HR_T(device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf()));

	// 3. 파이프 라인에 바인딩할 정점 셰이더 생성
	HR_T(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, skinnedMeshVertexShader.GetAddressOf()));

	vertexShaderBuffer.Reset();
	HR_T(CompileShaderFromFile(L"Shaders\\VS_DepthOnlyPass.hlsl", "main", "vs_5_0", vertexShaderBuffer.GetAddressOf()));
	HR_T(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, shadowMapVS.GetAddressOf()));

	// 5. 파이프라인에 바인딩할 픽셀 셰이더 생성
	ComPtr<ID3DBlob> pixelShaderBuffer = nullptr;
	pixelShaderBuffer.Reset();
	HR_T(CompileShaderFromFile(L"Shaders\\PS_DepthOnlyPass.hlsl", "main", "ps_5_0", pixelShaderBuffer.GetAddressOf()));
	HR_T(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, shadowMapPS.GetAddressOf()));

	pixelShaderBuffer.Reset();
	HR_T(CompileShaderFromFile(L"Shaders\\PS_PBR.hlsl", "main", "ps_5_0", pixelShaderBuffer.GetAddressOf()));
	HR_T(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, PbrPS.GetAddressOf()));

	return true;
}

void PBR1App::ShowMatrix(const DirectX::XMFLOAT4X4& mat, const char* label)
{
	// 각 행을 출력
	ImGui::Text("%.3f  %.3f  %.3f  %.3f", mat._11, mat._12, mat._13, mat._14);
	ImGui::Text("%.3f  %.3f  %.3f  %.3f", mat._21, mat._22, mat._23, mat._24);
	ImGui::Text("%.3f  %.3f  %.3f  %.3f", mat._31, mat._32, mat._33, mat._34);
	ImGui::Text("%.3f  %.3f  %.3f  %.3f", mat._41, mat._42, mat._43, mat._44);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT PBR1App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}

void PBR1App::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
	__super::OnInputProcess(KeyState, KeyTracker, MouseState, MouseTracker);

	if(KeyState.O)
	{
		auto model = make_unique<SkeletalModel>();
		if (!model->Load(hwnd, device, deviceContext, "..\\Resource\\char.fbx"))
		{
			MessageBox(hwnd, L"FBX file is invaild at path", NULL, MB_ICONERROR | MB_OK);
		}

		model->GetBuffer(transformBuffer, bonePoseBuffer, boneOffsetBuffer);

		Vector3 pos = freeCamera.position;
		model->position = pos;
		objects.push_back(std::move(model));
	}

	if (KeyState.P)
	{
		if (!objects.empty())
		{
			objects.front()->isRemoved = true;
			objects.pop_front();
		}
	}

	if (KeyState.Delete)
	{
		for (auto& e : objects)
		{
			e->isRemoved = true;
		}

		objects.clear();
	}
}