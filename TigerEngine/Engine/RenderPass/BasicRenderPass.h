// 251229
// TODO 일단 forward로 만들어보고 나중에 GbufferRenderPass로 변경하기
// 나중에 이 클래스를 abstract 클래스로 만들고 패스 만들어보기

#pragma once
#include "pch.h"
#include "../../Common/Renderer/IRenderPass.h"

class BasicRenderPass : public IRenderPass
{
private:
	ComPtr<ID3D11InputLayout> 			inputLayout{};
	ComPtr<ID3D11Buffer> 				cbuffer{};
	ComPtr<ID3D11SamplerState> 			sampleLinear{};

	ComPtr<ID3D11Texture2D> 			backbufferTexture{};
	ComPtr<ID3D11RenderTargetView> 		renderTargetView{};
	ComPtr<ID3D11ShaderResourceView> 	shaderResourceView{};	
	ComPtr<ID3D11DepthStencilView>		depthStencliView{};

	ComPtr<ID3D11PixelShader> 			pixelShader{};
	ComPtr<ID3D11VertexShader> 			vertexShader{};
	ComPtr<ID3D11BlendState>			blendState{};

	D3D11_VIEWPORT 						renderViewport = {};

public:
	virtual void Init(ComPtr<ID3D11Device>& device);
	void Execute( ComPtr<ID3D11DeviceContext>& context, 
					std::shared_ptr<Scene> scene,
					std::shared_ptr<Camera> cam) override;

protected:
	virtual void CreateEffect(ComPtr<ID3D11Device>& device);
};