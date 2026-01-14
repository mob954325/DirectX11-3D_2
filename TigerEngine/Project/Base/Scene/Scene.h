#pragma once
#include "../pch.h"
#include "../Renderer/RenderQueue.h"
#include "../Entity/RenderComponent.h"
#include <map>

class GameObject;

class Scene
{
public:
	void OnRender(std::unique_ptr<RenderQueue>& renderQueue);
	void OnUpdate(float deltaTime);	
	void CheckDestroy();

	void ForEachGameObject(std::function<void(GameObject*)> fn);

	void AddGameObject(GameObject* obj);
	GameObject* AddGameObjectByName(std::string name); // add empty gameObject to Scene
	GameObject* GetGameObjectByName(std::string name);

	void AddRenderable(RenderComponent* comp);
	std::vector<RenderComponent*>& GetRenderables();

	/// @brief 모든 씬 오브젝트들을 제거하는 함수
	void ClearScene();

	/// @brief json으로 scene정보를 저장하는 함수
	/// @param filename 저장할 파일 이름
	bool SaveToJson(const std::string& filename) const;

	/// @brief json 파일을 읽어서 scene에 로드하는 함수
	/// @param 불러오는 파일 이름
	bool LoadToJson(const std::string& filename);
	
	int GetObjectCount() { return gameObjects.size(); }
	GameObject* GetGameobjectFromScene(std::string name);

	GameObject* RayCastGameObject(const Ray& ray, float* outDistance);

protected:
	// std::vector<GameObject> gameObjects;
	std::multimap<std::string, GameObject*> gameObjects; // TODO 나중에 어떻게 관리할지 정하기
	std::vector<RenderComponent*> renderableComponents;
};