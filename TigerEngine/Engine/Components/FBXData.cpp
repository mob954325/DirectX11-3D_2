#include "FBXData.h"
#include "Helper.h"
#include <Manager/ComponentFactory.h>

REGISTER_COMPONENT(FBXData);

void FBXData::OnInitialize()
{
    // 임시
    fbxAsset = FBXResourceManager::Instance().LoadFBXByPath("Assets/Resource/sphere.fbx");
    meshes = fbxAsset->meshes; // pScene이 NULL 반환되서 Error남
}

const std::vector<Mesh>& FBXData::GetMesh() const
{
    return meshes;
}