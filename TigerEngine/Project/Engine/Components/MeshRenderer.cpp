#include "MeshRenderer.h"
#include "../Commands/DrawMeshCommand.h"
#include "System/ComponentFactory.h"
#include "Scene/Scene.h"
#include "../Components/FBXData.h"

void MeshRenderer::OnInitialize()
{
    auto comp = owner->GetComponent<FBXData>().lock();
    if(comp)
    {
        SetData(comp->GetFBXInfo());
    }
}

void MeshRenderer::OnStart()
{
}

void MeshRenderer::OnUpdate(float delta)
{
}

void MeshRenderer::SetData(std::shared_ptr<FBXResourceAsset> data)
{
    targetMeshData = data;

    auto comm = std::make_shared<DrawMeshCommand>();  
    comm->CreateCommand(data->meshes);   // 커멘드 생성
    command = comm; // 현재 커멘드 저장 
    SetCommand(command); 
}