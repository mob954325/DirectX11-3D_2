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
    path = "Assets\\Resource\\char.fbx";
    fbxAsset = FBXResourceManager::Instance().LoadFBXByPath("Assets\\Resource\\sphere.fbx");
    meshes = fbxAsset->meshes; 

    auto renderer = owner->GetComponent<FBXRenderer>(); // TODO load 후 컴포넌트 추가할 때 터짐 이거 수정하면 이 주석 제거할 것
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

nlohmann::json FBXData::Serialize()
{
    nlohmann::json datas;

    rttr::type t = rttr::type::get(*this);
    datas["type"] = t.get_name().to_string();
    datas["properties"] = nlohmann::json::object();

    for(auto& prop : t.get_properties())
    {
        std::string propName = prop.get_name().to_string();
        rttr::variant value = prop.get_value(*this);
        if(value.is_type<std::string>() && propName == "DataPath")
        {
            auto v = value.get_value<std::string>();
            datas["properties"][propName] = v;
        }
	}

    return datas;
}

void FBXData::Deserialize(nlohmann::json data)
{
    // data : data["objects"]["properties"]["components"]["현재 컴포넌트"]

    auto propData = data["properties"];

    rttr::type t = rttr::type::get(*this);
    for(auto& prop : t.get_properties())
    {
        std::string propName = prop.get_name().to_string();
        rttr::variant value = prop.get_value(*this);
	    if(value.is_type<std::string>() && propName == "DataPath")
        {
            std::string str = propData["DataPath"];
            prop.set_value(*this, str);
        }
	}
}
