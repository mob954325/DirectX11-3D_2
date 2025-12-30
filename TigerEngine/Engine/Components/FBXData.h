#pragma once
#include <pch.h>
#include <Entity/IComponent.h>
#include <Datas/Mesh.h>
#include <Manager/FBXResourceManager.h>

class FBXData : public IComponent
{
public:
    FBXData();

    void OnInitialize() override;
    const std::vector<Mesh>& GetMesh() const;
    
    std::string path = ""; // fbx path data

protected:
    std::vector<Mesh> meshes; // 현재 컴포넌트가 들고 있는 mesh 정보
    std::shared_ptr<FBXResourceAsset> fbxAsset; // 현재 컴포넌트가 들고 있는 mesh 정보
};