#include "GameObject.h"

template<typename T>
std::shared_ptr<T> GameObject::AddComponent()
{
	static_assert(std::is_base_of_v<IComponent, T>,
		"T must inherit from IComponent"); // T는 IComponent를 상속받았는가?

	auto comp = std::make_shared<T>();
	components.push_back(comp);
	return comp;
}

std::string GameObject::GetName() const
{
    return name;
}

std::shared_ptr<Transform> GameObject::GetTransform() const
{
	return transform;
}

std::vector<std::weak_ptr<IComponent>> &GameObject::GetIComponents()
{
    return components;
}

std::vector<std::weak_ptr<IRenderComponent>> &GameObject::GetIRenderComponents()
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
