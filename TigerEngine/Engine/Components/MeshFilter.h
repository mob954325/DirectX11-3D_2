#pragma once
#include <pch.h>
#include <Entity/IComponent.h>
#include <Datas/Vertex.h>
#include <vector>

class MeshFilter : public IComponent
{
public:
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

    void CreateVertexBuffer(ComPtr<ID3D11Device>& dev);
    void CreateIndexBuffer(ComPtr<ID3D11Device>& dev);

protected:
    std::vector<Vertex> vertices;   // each vertex data on mesh
    std::vector<UINT> indices;      // each vertex index on mesh

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    UINT vertexCount;
    
    // shader, constbuffer, index buffer index ...
    // buffer??
};