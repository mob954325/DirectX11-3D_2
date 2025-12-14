#pragma once
#include "../pch.h"
#include "../Entity/GameObject.h"
#include <map>

class Scene
{
public:
	void OnRender();
	void OnUpdate();

protected:
	std::map<std::string, std::shared_ptr<GameObject>> gameObjects; // 占싱몌옙, 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙트
};