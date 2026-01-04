#pragma once
#include "pch.h"
#include "../../Common/Renderer/IRenderPass.h"
#include "RenderPass/GBufferRenderPass.h"

class DirectionalLightPass : public IRenderPass
{
public:
	virtual void Init(ComPtr<ID3D11Device>& device);
	void Execute( ComPtr<ID3D11DeviceContext>& context, 
					std::shared_ptr<Scene> scene,
					std::shared_ptr<Camera> cam) override;    
    void End(ComPtr<ID3D11DeviceContext>& context) override;

    void SetClient(UINT width, UINT height);
    void SetGBufferSRV(std::vector<ComPtr<ID3D11ShaderResourceView>>& srvs);
    void SetDepthStencilView(ComPtr<ID3D11DepthStencilView>& dsv);
    void SetRenderTargetView(ComPtr<ID3D11RenderTargetView>& rtv);

private:
	ComPtr<ID3D11InputLayout> 			inputLayout{};
	ComPtr<ID3D11SamplerState> 			samplerLinear{};
    ComPtr<ID3D11SamplerState>          samplerPoint{};

	ComPtr<ID3D11Texture2D> 			backbufferTexture{};
	ComPtr<ID3D11RenderTargetView> 		renderTargetView{};
	ComPtr<ID3D11ShaderResourceView> 	shaderResourceView{};	
    
	ComPtr<ID3D11DepthStencilView>		depthStencliView{};
    ComPtr<ID3D11DepthStencilState>     dpethStencliState{};
	
	ComPtr<ID3D11PixelShader> 			pixelShader{};
	ComPtr<ID3D11VertexShader> 			vertexShader{};
	ComPtr<ID3D11BlendState>			blendState{}; 
    ComPtr<ID3D11RasterizerState>       rasterizerState{};
	
	ComPtr<ID3D11Buffer> 				lightDirectionBufferCB{};
	
	D3D11_VIEWPORT 						renderViewport = {};
    UINT clientWidth{};
    UINT clientHeight{};

    std::vector<ComPtr<ID3D11ShaderResourceView>> gbufferSRVs{};

    // PBR IBL 텍스쳐
    ComPtr<ID3D11ShaderResourceView> IBLIrradiance;
	ComPtr<ID3D11ShaderResourceView> IBLSpecular;
	ComPtr<ID3D11ShaderResourceView> IBLLookUpTable;

    // lights
    Color lightColor{};
    Vector3 lightDir{1, 1, 1};
    float lightIntensity = 1;
    
    // quad
    ComPtr<ID3D11Buffer> quadVertexBuffer{};
    ComPtr<ID3D11Buffer> quadIndexBuffer{};
    UINT quadVertexBufferStride{};
    UINT quadVertexBufferOffset{};
    int quadIndicesCount{};
    void CreateQuad(ComPtr<ID3D11Device> &device);    
};