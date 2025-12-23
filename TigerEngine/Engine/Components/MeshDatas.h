#pragma once
#include <pch.h>
#include <Entity/IComponent.h>
#include <Datas/Mesh.h>

class MeshDatas : public IComponent
{
public:
    void OnInitialize() override;
    const std::shared_ptr<Mesh>& GetMesh() const;

protected:
    std::shared_ptr<Mesh> mesh; // 현재 컴포넌트가 들고 있는 mesh 정보
};