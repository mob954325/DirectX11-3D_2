#include "DrawAABBBox.h"

void DrawAABBBox::Execute(ComPtr<ID3D11DeviceContext> &context)
{		
    //// 렌더타겟 다시 설정 (ImGui가 변경했을 수 있음)
	//context->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
	//
	//// DebugDraw의 BasicEffect 설정
	//DebugDraw::g_BatchEffect->SetWorld(Matrix::Identity);
	//DebugDraw::g_BatchEffect->SetView(m_View);
	//DebugDraw::g_BatchEffect->SetProjection(m_Projection);
	//DebugDraw::g_BatchEffect->Apply(context.Get());
	//
	//// InputLayout 설정
	//context->IASetInputLayout(DebugDraw::g_pBatchInputLayout.Get());
	//
	//// 블렌드 스테이트 설정 (깊이 테스트 활성화)
	//context->OMSetBlendState(DebugDraw::g_States->AlphaBlend(), nullptr, 0xFFFFFFFF);
	//context->OMSetDepthStencilState(DebugDraw::g_States->DepthRead(), 0);
	//context->RSSetState(DebugDraw::g_States->CullNone());
	//DebugDraw::g_Batch->Begin();
	//
	//// 선택된 오브젝트는 밝은 초록색
	//XMVECTOR color = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	//DebugDraw::Draw(DebugDraw::g_Batch.get(), m_pSelectedObject->m_AABB, color);
	//DebugDraw::g_Batch->End();
}