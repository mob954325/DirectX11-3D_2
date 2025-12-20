#pragma once
#include "../pch.h"
#include "../Entity/GameObject.h"
#include "Renderer/RenderQueue.h"
#include <map>

class Scene
{
public:
	void OnRender(std::unique_ptr<RenderQueue>& renderQueue);
	void OnUpdate(float deltaTime);	
	void CheckDestroy();

	void ForEachGameObject(std::function<void(std::shared_ptr<GameObject>)> fn);
	std::shared_ptr<GameObject> AddGameObject(std::string name); // add empty gameObject to Scene
	std::shared_ptr<GameObject> GetGameObjectByName(std::string name);

protected:
	std::multimap<std::string, std::shared_ptr<GameObject>> gameObjects; // mapping gameobjects;
};