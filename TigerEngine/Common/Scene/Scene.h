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

	/// @brief 모든 씬 오브젝트들을 제거하는 함수
	void ClearScene();

	/// @brief json으로 scene정보를 저장하는 함수
	/// @param filename 저장할 파일 이름
	bool SaveToJson(const std::string& filename) const;

	/// @brief json 파일을 읽어서 scene에 로드하는 함수
	/// @param 불러오는 파일 이름
	bool LoadToJson(const std::string& filename);

	
	int GetObjectCount() { return gameObjects.size(); }
	std::weak_ptr<GameObject> GetGameobjectFromScene(std::string name);

protected:
	std::multimap<std::string, std::shared_ptr<GameObject>> gameObjects; // TODO 나중에 어떻게 관리할지 정하기
	std::vector<std::weak_ptr<RenderComponent>> renderableComponents;
};