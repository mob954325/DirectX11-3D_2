#include "ShadowRenderPass.h"
#include "Entity/GameObject.h"

struct ConstantBuffer   // TODO 정리하기
{
	Matrix cameraView;
	Matrix cameraProjection;

	Matrix shadowView;
	Matrix shadowProjection;

	Vector4 ambient;	// 환경광
	Vector4 diffuse;	// 난반사
	Vector4 specular;	// 정반사
	FLOAT shininess;	// 광택지수
	Vector3 CameraPos;	// 카메라 위치
};

void ShadowRenderPass::Init(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& deviceContext, Camera* cam)
{
    /* ------------------------------- inputLayout ------------------------------ */
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
    ComPtr<ID3DBlob> vertexShaderBuffer{};
    HR_T(CompileShaderFromFile(L"Shaders\\VS_DepthOnlyPass.hlsl", "main", "vs_5_0", vertexShaderBuffer.GetAddressOf()));
    HR_T(device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf()));
    HR_T(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, vertexShader.GetAddressOf()));

    /* ----------------------------------- ps ----------------------------------- */
	ComPtr<ID3DBlob> pixelShaderBuffer = nullptr;
	pixelShaderBuffer.Reset();
	HR_T(CompileShaderFromFile(L"Shaders\\PS_DepthOnlyPass.hlsl", "main", "ps_5_0", pixelShaderBuffer.GetAddressOf()));
	HR_T(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, pixelShader.GetAddressOf()));

    /* --------------------------------- sampler -------------------------------- */
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// 텍스처 샘플링할 때 사용할 필터링 방법
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// 범위 밖에 있는 텍스처 좌표 확인 방법
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;	// 샘플링된 데이터를 기존 데이터와 확인하는 방법
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HR_T(device->CreateSamplerState(&sampDesc, samplerLinear.GetAddressOf()));

    /* ---------------------------- depthStencilView ---------------------------- */
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;                // 깊이 테스트 활성화
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 깊이 버퍼 업데이트 허용
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; // 작은 Z 값이 앞에 배치되도록 설정
	depthStencilDesc.StencilEnable = FALSE;            // 스텐실 테스트 비활성화

	device->CreateDepthStencilState(&depthStencilDesc, &depthStencliState);

    /* ---------------------------- shadowMapTexture ---------------------------- */
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
	HR_T(device->CreateTexture2D(&texDesc, nullptr, shadowMapTexture.GetAddressOf()));

	/* --------------------------------- // DSV --------------------------------- */
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	HR_T(device->CreateDepthStencilView(shadowMapTexture.Get(), &descDSV, depthStencliView.GetAddressOf()));

	/* --------------------------------- // SRV --------------------------------- */
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	HR_T(device->CreateShaderResourceView(shadowMapTexture.Get(), &srvDesc, shaderResourceView.GetAddressOf()));

    /* -------------------------------- 상수 버퍼 만들기 ------------------------------- */
    D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(ConstantBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	HR_T(device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf()));

	/* ----------------------------- // 빛 계산 ( pov ) ---------------------------- */
    camera = cam;
    auto camTran = camera->GetOwner()->GetTransform().lock();
	shadowProj = XMMatrixPerspectiveFovLH(shadowFrustumAngle, shadowViewport.width / (FLOAT)shadowViewport.height, shadowNear, shadowFar); // 그림자 절두체
	shadowLookAt = camTran->position + camera->GetForward() * shadowForwardDistFromCamera;	// 바라보는 방향 = 카메라 위치 + 카메라 바라보는 방향으로부터 떨어진 태양의 위치
	shadowPos = camTran->position + ((Vector3)-lightDirection * shadowUpDistFromLookAt);	// 위치
	shadowView = XMMatrixLookAtLH(shadowPos, shadowLookAt, Vector3(0.0f, 1.0f, 0.0f));
}

void ShadowRenderPass::Execute(ComPtr<ID3D11DeviceContext> &context, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> cam)
{
    // 바인딩 해제
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	context->VSSetShaderResources(4, 1, nullSRV);
	context->PSSetShaderResources(4, 1, nullSRV);

	//상수 버퍼 갱신
	ConstantBuffer cb;
	cb.cameraView = XMMatrixTranspose(camera->GetView());
	cb.cameraProjection = XMMatrixTranspose(camera->GetProjection());
	cb.shadowView = XMMatrixTranspose(shadowView);
	cb.shadowProjection = XMMatrixTranspose(shadowProj);
	cb.CameraPos = camera->GetOwner()->GetTransform().lock()->position;

	// 뷰포트 설정 + DSV 초기화, RS, OM 설정
	D3D11_VIEWPORT viewport
	{
		shadowViewport.x, shadowViewport.y,
		shadowViewport.width, shadowViewport.height,
		shadowViewport.minDepth, shadowViewport.maxDepth
	};
	context->RSSetViewports(1, &viewport);
	context->OMSetDepthStencilState(depthStencliState.Get(), 1);
	context->OMSetRenderTargets(0, nullptr, depthStencliView.Get());
	context->ClearDepthStencilView(depthStencliView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// 렌더 파이프라인 설정
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout.Get());

	context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    
	context->PSSetSamplers(0, 1, samplerLinear.GetAddressOf());

	context->VSSetShader(vertexShader.Get(), 0, 0);
	// m_pDeviceContext->PSSetShader(NULL, NULL, 0); // 렌더 타겟에 기록할 RGBA가 없으므로 실행하지 않는다.
	context->PSSetShader(pixelShader.Get(), NULL, 0); // 
}

void ShadowRenderPass::End(ComPtr<ID3D11DeviceContext> &context)
{
	context->PSSetShaderResources(4, 1, shaderResourceView.GetAddressOf());
}