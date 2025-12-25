#include "MeshRenderer.h"
#include "Renderer/DrawMeshCommand.h"
#include "Manager/ComponentFactory.h"

REGISTER_COMPONENT(MeshRenderer);

void MeshRenderer::OnInitialize()
{
}

void MeshRenderer::OnStart()
{
}

void MeshRenderer::OnUpdate(float delta)
{
}

void MeshRenderer::OnRender(std::unique_ptr<RenderQueue>& queue)
{
    DrawMeshCommand command;
    vector<Mesh> meshes = targetMeshData->GetMesh();

    for(auto& e : meshes)
    {
        command.CreateCommand(e.vertices, e.indices);
        //queue->AddCommand()
    }
}
