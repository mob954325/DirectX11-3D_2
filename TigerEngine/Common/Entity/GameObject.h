#pragma once
#include "pch.h"
#include "Transform.h"
#include <string> 
#include <vector>
#include <Entity/RenderComponent.h>
#include <Scene/Scene.h>

/// <summary>
/// GameObject는 컴포넌트를 담고 있는 순수한 컨테이너
/// </summary>
class GameObject
{
public:
	GameObject() = delete;
	GameObject(Scene* scene, std::string name) : name(name) { Init(scene); }

	template<typename T>
	std::shared_ptr<T> AddComponent()
	{
		static_assert(std::is_base_of_v<IComponent, T>,
			"T must inherit from IComponent"); // T는 IComponent를 상속받았는가? 

		auto comp = std::make_shared<T>(this);//
		components.push_back(comp);
		comp->OnInitialize(); // 컴포넌트 초기화 실행

		if (auto renderComp = std::dynamic_pointer_cast<RenderComponent>(comp))
		{
			currentScene->AddRenderable(renderComp); // 렌더링하는 컴포넌트 등록
		}

		return comp;
	}
	
	std::string GetName() const;
	std::shared_ptr<Transform> GetTransform() const;
	std::vector<std::shared_ptr<IComponent>>& GetIComponents();
	
	bool IsDestory();
	void Destory();
	
	Scene* GetScene();

protected:
	Scene* currentScene{}; // 현재 게임 오브젝트가 존재하는 씬 참조 변수
	std::string name = "NoNamed";
	std::shared_ptr<Transform> transform;
	std::vector<std::shared_ptr<IComponent>> components;
	bool isDestory = false;

private:
	/// @brief 게임 오브젝트 생성 시 호출되는 함수
	void Init(Scene* scene);
};

