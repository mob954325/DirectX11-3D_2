#include "MeshRenderer.h"
#include "Renderer/DrawMeshCommand.h"

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
    // queue->AddCommand()
}
