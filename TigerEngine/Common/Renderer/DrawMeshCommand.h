#include "pch.h"
#include <Renderer/IRenderCommand.h>

class DrawMeshCommand : public IRenderCommand
{
private:
    ComPtr<ID3D11Buffer> vertexBuffer;
    UINT vertexCount;
    // shader, constbuffer, index buffer index ...

public:
    void Execute(ComPtr<ID3D11DeviceContext> context) override
    {
        // call Render functions,,
        // context->IASetVertexBuffers(...)
        // context->Draw(vertexCount, 0);
    }
};