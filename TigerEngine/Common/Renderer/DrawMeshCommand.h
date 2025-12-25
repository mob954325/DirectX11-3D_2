#include "pch.h"
#include <Renderer/IRenderCommand.h>
#include <Datas/Vertex.h>

class DrawMeshCommand : public IRenderCommand
{
private:

public:
    void CreateCommand(std::vector<BoneWeightVertex> verteices, std::vector<UINT> indexes);

    void Execute(ComPtr<ID3D11DeviceContext> context) override
    {
        // TODO 여기에 간단한 렌더 세팅하고 메쉬 출력 확인하기
        // call Render functions,,
        // context->IASetVertexBuffers(...)
        // context->Draw(vertexCount, 0);
    }
};