#pragma once
#include "../pch.h"
#include "Renderer/RenderQueue.h"
#include <Entity/RenderComponent.h>
#include <map>

class GameObject;

class Scene
{
public:
	void OnRender(std::unique_ptr<RenderQueue>& renderQueue);
	void OnUpdate(float deltaTime);	
	void CheckDestroy();

	void ForEachGameObject(std::function<void(std::shared_ptr<GameObject>)> fn);

	void AddGameObject(std::shared_ptr<GameObject> obj);
	std::shared_ptr<GameObject> AddGameObjectByName(std::string name); // add empty gameObject to Scene
	std::shared_ptr<GameObject> GetGameObjectByName(std::string name);

	void AddRenderable(std::shared_ptr<RenderComponent> comp);
	std::vector<std::weak_ptr<RenderComponent>>& GetRenderables();

protected:
	std::multimap<std::string, std::shared_ptr<GameObject>> gameObjects; // mapping gameobjects;
	std::vector<std::weak_ptr<RenderComponent>> renderableComponents;
};