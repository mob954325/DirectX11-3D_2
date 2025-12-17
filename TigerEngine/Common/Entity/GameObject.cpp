#include "GameObject.h"

GameObject::GameObject()
{
	AddComponent<Transform>();
}

template<typename T>
std::shared_ptr<T> GameObject::AddComponent()
{
	static_assert(std::is_base_of_v<IComponent, T>,
		"T must inherit from IComponent"); // T는 IComponent를 상속받았는가?

	auto comp = std::make_shared<T>();
	components.push_back(comp);
	return comp;
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
