#pragma once
#include "pch.h"
#include "Renderer/IRenderCommand.h"
#include "Datas/Mesh.h"

class DrawMeshCommand : public IRenderCommand
{
private:
    std::vector<Mesh> refMesh{};
public:
    void CreateCommand(std::vector<Mesh>& meshes);

    void Execute(ComPtr<ID3D11DeviceContext>& context) override
    {
        if(refMesh.empty()) return;

        for(auto& mesh : refMesh)
        {
            mesh.Draw(context); // call draw from mesh
        }
    }
};