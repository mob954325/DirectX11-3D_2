#include "MeshDatas.h"
#include "Helper.h"
#include <Manager/ComponentFactory.h>

REGISTER_COMPONENT(MeshDatas);

void MeshDatas::OnInitialize()
{
    // 임시
    fbxAsset = FBXResourceManager::Instance().LoadFBXByPath("Assets/Resource/sphere.fbx");
    meshes = fbxAsset->meshes;
}

const std::vector<Mesh>& MeshDatas::GetMesh() const
{
    return meshes;
}