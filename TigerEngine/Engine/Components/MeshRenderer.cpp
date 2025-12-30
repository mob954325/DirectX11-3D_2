#include "MeshRenderer.h"
#include "Renderer/DrawMeshCommand.h"
#include "Manager/ComponentFactory.h"
#include "Scene/Scene.h"

REGISTER_COMPONENT(MeshRenderer);

void MeshRenderer::OnInitialize()
{
    owner->GetScene()->AddRenderable()
}

void MeshRenderer::OnStart()
{
}

void MeshRenderer::OnUpdate(float delta)
{
}