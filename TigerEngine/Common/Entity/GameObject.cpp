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

std::vector<std::shared_ptr<IRenderComponent>> &GameObject::GetIRenderComponents()
{
    return renderComponents;
}

bool GameObject::IsDestory()
{
    return isDestory;
}

void GameObject::Destory()
{
	isDestory = true;
}

void GameObject::Init()
{
	transform = AddComponent<Transform>();
}
