#pragma once
#include "../pch.h"
#include "Transform.h"
#include <string> 
#include <vector>
#include "../Scene/Scene.h"
#include "../Entity/RenderComponent.h"
class RenderComponent; // NOTE : IComponent 있는 거랑 순환 참조 조심하기

/// <summary>
/// GameObject는 컴포넌트를 담고 있는 순수한 컨테이너
/// </summary>
class GameObject
{
public:
	GameObject() { Initialize(); };
	GameObject(Scene* scene, std::string name) : GameObject()
	{ 
		this->name = name;
		SetScene(scene); 
	}

	template<typename T>
	std::weak_ptr<T> AddComponent();	
		
	template<typename T>
	std::weak_ptr<T> GetComponent();

	void RemoveComponent(std::weak_ptr<IComponent> comp);

	std::weak_ptr<Transform> GetTransform() const;
	std::vector<std::shared_ptr<IComponent>> GetIComponents();
	
	bool IsDestory();
	void Destory();
	
	std::string GetName() const;
	void SetName(std::string str);
	
	Scene* GetScene();
	/// @brief 게임 오브젝트가 포함될 씬 설정 함수
	void SetScene(Scene* scene);

	/// @brief 데이터 저장을 위한 직렬화 데이터 반환 함수
	/// @return 직렬화된 json 객체
	nlohmann::json Serialize() const;
	void Deserialize(const nlohmann::json objData);

	/// @brief 에디터 오브젝트 피킹을 위한 AABB 업데이트
	void UpdateAABB();
	const BoundingBox GetAABB() { return aabbBox; }

	std::string name = "NoNamed";	// 리플렉션을 위해 public으로 공개

protected:
	Scene* currentScene{}; // 현재 게임 오브젝트가 존재하는 씬 참조 변수
	std::weak_ptr<Transform> transform{};
	std::vector<std::shared_ptr<IComponent>> 	components;
	bool isDestory = false;

	BoundingBox aabbBox{};

	void Initialize();
};

template <typename T>
inline std::weak_ptr<T> GameObject::AddComponent()
{
	static_assert(std::is_base_of_v<IComponent, T>,
		"T must inherit from IComponent"); // T는 IComponent를 상속받았는가? 

	auto comp = std::make_shared<T>();
	comp->SetOwner(this);
	components.push_back(comp);
	comp->OnInitialize(); // 컴포넌트 초기화 실행

	if (auto renderComp = std::dynamic_pointer_cast<RenderComponent>(comp))
	{
		currentScene->AddRenderable(renderComp); // 렌더링하는 컴포넌트 등록
	}

	return comp;
}

template <typename T>
inline std::weak_ptr<T> GameObject::GetComponent()
{
	std::weak_ptr<T> res = {};
	std::for_each(components.begin(), components.end(), [&res](auto comp)
	{
		if(typeid(*comp) == typeid(T)) res = std::dynamic_pointer_cast<T>(comp);
	});

    return res;
}
