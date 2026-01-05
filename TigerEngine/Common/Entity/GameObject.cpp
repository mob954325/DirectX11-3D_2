#include "GameObject.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<GameObject>("GameObject")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr) // ??
        .property("GameObject", &GameObject::name)
        .property("Value", &GameObject::value);
}

std::string GameObject::GetName() const
{
    return name;
}

void GameObject::SetName(std::string str)
{
    name = str;
}

std::shared_ptr<Transform> GameObject::GetTransform() const
{
	return transform;
}

std::vector<std::shared_ptr<IComponent>> &GameObject::GetIComponents()
{
    return components;
}

bool GameObject::IsDestory()
{
    return isDestory;
}

void GameObject::Destory()
{
	isDestory = true;
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
