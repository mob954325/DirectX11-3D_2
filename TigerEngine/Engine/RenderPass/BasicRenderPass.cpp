#include "pch.h"
#include "BasicRenderPass.h"

void BasicRenderPass::Execute(std::shared_ptr<ID3D11DeviceContext> context, const Scene &scene)
{
    // TODO 상수버퍼 넣기,

	// 텍스처 및 샘플링 설정 
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_pInputLayout.Get());
    
	context->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());

	context->PSSetShader(m_pBlinnPhongShader.Get(), 0, 0);

	// context->PSSetShader(m_pPixelShader.Get(), 0, 0);
	context->PSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(1, 1, m_pMaterialBuffer.GetAddressOf());

	context->PSSetSamplers(0, 1, m_pSamplerLinear.GetAddressOf());
}