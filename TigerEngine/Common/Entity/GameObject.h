#pragma once
#include "pch.h"
#include "Transform.h"
#include "IComponent.h"
#include "IRenderComponent.h"
#include <string>
#include <vector>

/// <summary>
/// GameObject는 컴포넌트를 담고 있는 순수한 컨테이너
/// </summary>
class GameObject
{
public:
	GameObject() { Init(); }
	GameObject(std::string name) : name(name) { Init(); }

	template<typename T>
	std::shared_ptr<T> AddComponent()
	{
		static_assert(std::is_base_of_v<IComponent, T>,
		"T must inherit from IComponent"); // T는 IComponent를 상속받았는가?

		auto comp = std::make_shared<T>();
		components.push_back(comp);
		comp->OnInitialize(); // 컴포넌트 초기화 실행

		return comp;
	}
	
	std::string GetName() const;
	std::shared_ptr<Transform> GetTransform() const;
	std::vector<std::shared_ptr<IComponent>>& GetIComponents();
	std::vector<std::shared_ptr<IRenderComponent>>& GetIRenderComponents();

	bool IsDestory();
	void Destory();

protected:
	std::string name = "NoNamed";
	std::shared_ptr<Transform> transform;
	std::vector<std::shared_ptr<IComponent>> components; // Update, Physics 등
	std::vector<std::shared_ptr<IRenderComponent>> renderComponents; // Mesh, Material, Color 등등
	bool isDestory = false;

private:
	/// @brief 게임 오브젝트 생성 시 호출되는 함수
	void Init();
};

