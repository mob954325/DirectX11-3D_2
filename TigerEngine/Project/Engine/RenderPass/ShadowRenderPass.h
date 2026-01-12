#pragma once
#include "pch.h"
#include "Renderer/IRenderPass.h"
#include "Entity/Camera.h"

class ShadowRenderPass : public IRenderPass
{
public:
	virtual void Init(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& deviceContext, Camera* cam);
	void Execute( ComPtr<ID3D11DeviceContext>& context, 
					std::shared_ptr<Scene> scene,
					Camera* cam) override;    
    void End(ComPtr<ID3D11DeviceContext>& context) override;

	ComPtr<ID3D11ShaderResourceView> GetShadowSRV() { return shaderResourceView; };
	Matrix GetShadowView() { return shadowView; }
	Matrix GetShadowProjection() { return shadowProj; }

private:
	ComPtr<ID3D11InputLayout> 			inputLayout{};
	ComPtr<ID3D11SamplerState> 			samplerLinear{};

	ComPtr<ID3D11Texture2D> 			shadowMapTexture{};
	ComPtr<ID3D11RenderTargetView> 		renderTargetView{};
	ComPtr<ID3D11ShaderResourceView> 	shaderResourceView{};	
    
	ComPtr<ID3D11DepthStencilView>		depthStencliView{};
    ComPtr<ID3D11DepthStencilState>     depthStencliState{};
	
	ComPtr<ID3D11PixelShader> 			pixelShader{};
	ComPtr<ID3D11VertexShader> 			vertexShader{};
	ComPtr<ID3D11BlendState>			blendState{}; 
    ComPtr<ID3D11RasterizerState>       rasterizerState{};
	
    ComPtr<ID3D11Buffer> 				constantBuffer{};

    Camera* camera{};
    
	Matrix shadowView{};
	Matrix shadowProj{};
	Vector3 shadowLookAt{};
	Vector3 shadowPos{};
    Vector3 shadowUpDistFromLookAt{ 1000, 1000, 1000 };
	float shadowFrustumAngle = XM_PIDIV4;

	Viewport shadowViewport = { 0, 0, 8192, 8192, 0.0f, 1.0f }; // x, y, width, height, min, max
	D3D11_VIEWPORT RenderViewport = {};
	float shadowForwardDistFromCamera = 1.0f;
	float shadowNear = 400.0f;
	float shadowFar = 3000.0f;
	const float shadowMinNear = 400.0f;
	const float shadowMinFar = 1001.0f;

    Vector4 lightDirection{ 0, -1, 0, 1}; // TODO 나중에 world 세팅으로 lightDirection 패스마다 분리된거 없애기
};