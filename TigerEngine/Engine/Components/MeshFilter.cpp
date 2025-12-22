#include "MeshFilter.h"
#include "Helper.h"

void MeshFilter::OnInitialize()
{

}

void MeshFilter::OnStart()
{
}

void MeshFilter::OnUpdate(float delta)
{
}

void MeshFilter::CreateVertexBuffer(ComPtr<ID3D11Device> &dev)
{
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = &vertices[0];

    HR_T(dev->CreateBuffer(&vbd, &initData, vertexBuffer.GetAddressOf()));
}

void MeshFilter::CreateIndexBuffer(ComPtr<ID3D11Device> &dev)
{
        // index buffer
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = &indices[0];
    HR_T(dev->CreateBuffer(&ibd, &initData, indexBuffer.GetAddressOf()));
}
