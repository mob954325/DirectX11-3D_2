#pragma once
#include <Entity/IComponent.h>
#include <Datas/Vertex.h>
#include <vector>

class MeshFilter : public IComponent
{
public:
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

protected:
    std::vector<Vertex> vertices;   // each vertex data on mesh
    std::vector<UINT> indices;      // each vertex index on mesh
};