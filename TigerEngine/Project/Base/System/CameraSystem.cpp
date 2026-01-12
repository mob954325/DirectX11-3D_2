#include "../System/CameraSystem.h"
#include "../Entity/GameObject.h"
#include "../Entity/Camera.h"

Camera *CameraSystem::GetFreeCamera()
{
    return freeCamera;
}

void CameraSystem::Register(Camera *cam)
{
    registered.push_back(cam);
    mappedRegistered.insert({cam->GetOwner()->GetName(), cam });
}

void CameraSystem::RemoveCamera(Camera *cam)
{
    // vector에서 제거
    for(auto it = registered.begin(); it != registered.end();)
    {
        if(*it == cam)
        {
            auto vback = registered.back();
            registered.back() = *it;
            *it = vback;
            registered.pop_back();
        }
        it++;
    }

    // map에서 제거
    std::string name = cam->GetOwner()->GetName();
    mappedRegistered.erase(name);
}

Camera *CameraSystem::GetCameraByObjectName(std::string name)
{
    if(auto it = mappedRegistered.find(name); it != mappedRegistered.end())
    {
        return it->second;
    }
    else
    {

        return nullptr;
    }
}

void CameraSystem::Clear()
{
    registered.clear();
    mappedRegistered.clear();
}

void CameraSystem::FreeCameraUpdate(float delta)
{
    if(!freeCamObj) return;
    auto comps = freeCamObj->GetIComponents();
    for(auto& comp : comps)
    {
        comp->OnUpdate(delta);
    }
}

void CameraSystem::CreateFreeCamera(int clientWidth, int clientHeight, Scene *currScene)
{
    // TODO 카메라 씬에 등록하기 아니면 다른 업데이트 방법 찾기
    freeCamObj = std::make_shared<GameObject>();
	freeCamObj->SetName("FreeCamera");
    freeCamera = freeCamObj->AddComponent<Camera>().lock().get();

	freeCamera->SetProjection(DirectX::XM_PIDIV2, clientWidth, clientHeight, 0.1, 1000);
    Register(freeCamera);
}
