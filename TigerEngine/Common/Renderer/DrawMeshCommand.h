#include "pch.h"
#include <Renderer/IRenderCommand.h>
#include <Datas/Vertex.h>

class DrawMeshCommand : public IRenderCommand
{
private:
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    UINT vertexCount;
    // shader, constbuffer, index buffer index ...

public:
    void CreateCommand(std::vector<Vertex> verteices, std::vector<UINT> indexes);

    void Execute(ComPtr<ID3D11DeviceContext> context) override
    {
        // call Render functions,,
        // context->IASetVertexBuffers(...)
        // context->Draw(vertexCount, 0);
    }
};