#include "FBXData.h"
#include "Helper.h"
#include <System/ComponentFactory.h>
#include <Components/FBXRenderer.h>

RTTR_REGISTRATION
{
    rttr::registration::class_<FBXData>("FBXData")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr)
        .property("DataPath", &FBXData::path);
}

REGISTER_COMPONENT(FBXData);

void FBXData::OnInitialize()
{
    // 임시
    path = "Assets/Resource/char.fbx";
    fbxAsset = FBXResourceManager::Instance().LoadFBXByPath("Assets/Resource/char.fbx");
    meshes = fbxAsset->meshes; 

    auto renderer = owner->GetComponent<FBXRenderer>();
    if(!renderer.expired()) renderer.lock()->OnInitialize();
}

const std::vector<Mesh>& FBXData::GetMesh() const
{
    return meshes;
}

const std::shared_ptr<FBXResourceAsset> FBXData::GetFBXInfo() const
{
    return fbxAsset;
}
