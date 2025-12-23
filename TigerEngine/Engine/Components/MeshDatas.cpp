#include "MeshDatas.h"
#include "Helper.h"
#include <Manager/ComponentFactory.h>

REGISTER_COMPONENT(MeshDatas);

void MeshDatas::OnInitialize()
{

}

const std::shared_ptr<Mesh> &MeshDatas::GetMesh() const
{
    return mesh;
}