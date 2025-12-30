#include "GameObject.h"

std::string GameObject::GetName() const
{
    return name;
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

void GameObject::Init(Scene* scene)
{
	transform = AddComponent<Transform>();
    currentScene = scene;
}
