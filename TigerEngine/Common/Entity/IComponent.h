#pragma once
#include <pch.h>

class GameObject; // forward declear

class IComponent
{
protected:
	GameObject* owner{}; // 해당 컴포넌트를 소유하고 있는 게임 오브젝트

public:
	IComponent() = default;
	virtual ~IComponent() = default;
	
	/// <summary>
	/// IComponent가 처음 실행될 때 실행됩니다.
	/// </summary>
	virtual void OnInitialize() {};

	/// <summary>
	/// OnUpdate()를 실행하기 전 ***한 번*** 실행됩니다.
	/// </summary>
	virtual void OnStart() {};

	/// <summary>
	/// GameEngine에 매 프레임마다 OnUpdate()내 에서 호출됩니다.
	/// </summary>
	virtual void OnUpdate(float delta) {};

	void SetOwner(GameObject* obj) { owner = obj; }
	GameObject* GetOwner() { return owner; }
};