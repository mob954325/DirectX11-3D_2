#include "DrawAABBBox.h"

void DrawAABBBox::Execute(ComPtr<ID3D11DeviceContext> &context)
{	
	// InputLayout 설정
	DebugDraw::g_Batch->Begin();
	
	// 선택된 오브젝트는 밝은 초록색
	XMVECTOR color = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	DebugDraw::Draw(DebugDraw::g_Batch.get(), targetObject->GetAABB(), color);
	DebugDraw::g_Batch->End();
}