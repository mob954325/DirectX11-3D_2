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

private:
	ComPtr<ID3D11InputLayout> 			inputLayout{};
	ComPtr<ID3D11SamplerState> 			samplerLinear{};

	ComPtr<ID3D11Texture2D> 			shadowMapTexture{};
	ComPtr<ID3D11ShaderResourceView> 	shaderResourceView{};	
    
	ComPtr<ID3D11DepthStencilView>		depthStencliView{};
    ComPtr<ID3D11DepthStencilState>     depthStencliState{};
	
	ComPtr<ID3D11PixelShader> 			pixelShader{};
	ComPtr<ID3D11VertexShader> 			vertexShader{};
	
    ComPtr<ID3D11Buffer> 				constantBuffer{};

    Camera* camera{};

	D3D11_VIEWPORT RenderViewport = {};
};