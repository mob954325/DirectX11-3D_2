#include "BasicRenderPass.h"
#include "Helper.h"

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
	FLOAT ambientOcclusion;
	FLOAT pad3;
};

void BasicRenderPass::Init(ComPtr<ID3D11Device>& device)
{
	/* ----------------------------- inputLayout 설정 ----------------------------- */	
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

	/* -------------------------------- 상수 버퍼 만들기 ------------------------------- */
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(ConstantBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	HR_T(device->CreateBuffer(&bufferDesc, nullptr, cbuffer.GetAddressOf()));
	
	ComPtr<ID3DBlob> vertexShaderBuffer{};
	HR_T(CompileShaderFromFile(L".\\Shaders\\VS_SkinnedMesh.hlsl", "main", "vs_5_0", vertexShaderBuffer.GetAddressOf()));
	HR_T(device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf()));
	
	/* --------------------------------- 버텍스 셰이더 만들기 -------------------------------- */

	// 3. 파이프 라인에 바인딩할 정점 셰이더 생성
	HR_T(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, vertexShader.GetAddressOf()));

	// vertexShaderBuffer.Reset();
	// HR_T(CompileShaderFromFile(L"Shaders\\VS_DepthOnlyPass.hlsl", "main", "vs_5_0", vertexShaderBuffer.GetAddressOf()));
	// HR_T(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, m_pShadowMapVS.GetAddressOf()));

	/* ------------------------------- 픽셀 세이더 만들기 ------------------------------- */

	ComPtr<ID3DBlob> pixelShaderBuffer{};
	//pixelShaderBuffer.Reset();
	//HR_T(CompileShaderFromFile(L"Shaders\\PS_DepthOnlyPass.hlsl", "main", "ps_5_0", pixelShaderBuffer.GetAddressOf()));
	//HR_T(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, m_pShadowMapPS.GetAddressOf()));

	pixelShaderBuffer.Reset();
	HR_T(CompileShaderFromFile(L"Shaders\\PS_PBR.hlsl", "main", "ps_5_0", pixelShaderBuffer.GetAddressOf()));
	HR_T(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, pixelShader.GetAddressOf()));


}

void BasicRenderPass::Execute(ComPtr<ID3D11DeviceContext> &context, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> cam)
{
		// TODO 하드 코딩 제거하고 카메라 불러오기
	// 카메라, 클라이언트 화면 크기, ...
	// 상수 버퍼 설정
	ConstantBuffer cb;
	Vector3 eye {0,0,-10};
	Vector3 dir {0,0,1};
	Matrix view = XMMatrixLookToLH(eye, dir, Vector3::Up);
	cb.cameraView = XMMatrixTranspose(view);

	Matrix projection = XMMatrixPerspectiveFovLH(1.3f, 1280 / (FLOAT)720, 0.1, 1000);
	cb.cameraProjection = XMMatrixTranspose(projection);
	// cb.lightDirection = m_LightDirection;
	// cb.lightDirection.Normalize();
	// cb.shadowView = XMMatrixTranspose(m_shadowView);
	// cb.shadowProjection = XMMatrixTranspose(m_shadowProj);
	// cb.lightColor = m_LightColor;
	// cb.ambient = m_LightAmbient;
	// cb.diffuse = m_LightDiffuse;
	// cb.specular = m_LightSpecular;
	// cb.shininess = m_Shininess;
	// cb.CameraPos = m_Camera.m_Position;

	// 텍스처 및 샘플링 설정 
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout.Get());    
	
	context->PSSetShader(pixelShader.Get(), 0, 0);
	
	context->VSSetConstantBuffers(0, 1, cbuffer.GetAddressOf());
	context->VSSetShader(vertexShader.Get(), 0, 0);
	context->PSSetConstantBuffers(0, 1, cbuffer.GetAddressOf());
	context->PSSetShader(pixelShader.Get(), 0, 0);
	
	context->PSSetSamplers(0, 1, sampleLinear.GetAddressOf());
}

void BasicRenderPass::CreateEffect(ComPtr<ID3D11Device>& device)
{
	// ...
}