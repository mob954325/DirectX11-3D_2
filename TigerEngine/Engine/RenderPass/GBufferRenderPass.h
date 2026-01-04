#pragma once
#include "pch.h"
#include "../../Common/Renderer/IRenderPass.h"

class GBufferRenderPass : public IRenderPass
{
public:
	void Init(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& deviceContext, UINT width, UINT height);
	void Execute( ComPtr<ID3D11DeviceContext>& context, 
					std::shared_ptr<Scene> scene,
					std::shared_ptr<Camera> cam) override;    
    void End(ComPtr<ID3D11DeviceContext>& context) override;
    void SetDepthStencilView(ComPtr<ID3D11DepthStencilView>& dsv);

    std::vector<ComPtr<ID3D11ShaderResourceView>>& GetShaderResourceViews();

private:
	ComPtr<ID3D11InputLayout> 			inputLayout{};
	ComPtr<ID3D11SamplerState> 			samplerLinear{};
    ComPtr<ID3D11SamplerState>          samplerPoint{};

    std::vector<ComPtr<ID3D11RenderTargetView>> gbufferRTVs; 
	std::vector<ComPtr<ID3D11ShaderResourceView>> gbufferSRVs;
	std::vector<ComPtr<ID3D11Texture2D>> gbufferTextures;

	ComPtr<ID3D11Texture2D> 			backbufferTexture{};
	ComPtr<ID3D11RenderTargetView> 		renderTargetView{};
	ComPtr<ID3D11ShaderResourceView> 	shaderResourceView{};	
    
    ComPtr<ID3D11DepthStencilView>      depthStencilView{};
    ComPtr<ID3D11DepthStencilState>     dpethStencliState{};
	
	ComPtr<ID3D11PixelShader> 			pixelShader{};
	ComPtr<ID3D11VertexShader> 			vertexShader{};
	// ComPtr<ID3D11BlendState>			blendState{}; 
    ComPtr<ID3D11RasterizerState>       rasterizerState{};
	
	ComPtr<ID3D11Buffer> 				cbCamera{};
	
	D3D11_VIEWPORT 						renderViewport = {};
    UINT clientWidth{};
    UINT clientHeight{};

    enum class EGbuffer
	{
		BaseColor = 0,
		Normal,
		WorldPos,
		Matal,
		Rough,
		Specular,
		Emission,
		Count
	};

    int bufferCount = 0; // GBuffer 사용하는 개수

    void CreateGBuffers(ComPtr<ID3D11Device> &device);
};