#include "GameObject.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<GameObject>("GameObject")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr) // ??
        .property("GameObject", &GameObject::name);
}

std::string GameObject::GetName() const
{
    return name;
}

void GameObject::SetName(std::string str)
{
    name = str;
}

void GameObject::RemoveComponent(std::weak_ptr<IComponent> comp)
{
    // 찾기
    for(auto it = components.begin(); it != components.end(); it++)
    {       
        if(*it == comp.lock())
        {
            components.erase(it);
            break;
        }
    }   
}

std::weak_ptr<Transform> GameObject::GetTransform() const
{
	return transform;
}

std::vector<std::shared_ptr<IComponent>> GameObject::GetIComponents()
{
    return components;
}

bool GameObject::IsDestory()
{
    return isDestory;
}

void GameObject::Destory()
{
	isDestory = true; // 가지고 있는 모든 컴포넌트 파괴하기
}

Scene *GameObject::GetScene()
{
    return currentScene;
}

void GameObject::SetScene(Scene* scene)
{
	transform = AddComponent<Transform>();
    currentScene = scene;
}
