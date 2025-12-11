#pragma once
#include "../../Common/pch.h"
#include "../../Common/GameApp.h"
#include "../../Common/Scene/Scene.h"
#include <map>

/// <summary>
/// 렌더 파이프라인이 흐름을 관리하는 앱
/// </summary>
class EngineApp : public GameApp
{
public:
	EngineApp(HINSTANCE hInstance);
	~EngineApp();

	bool OnInitialize() override;
	void OnUpdate() override;
	void OnRender() override;

	Scene scene; // TODO : 씬 교체할 수 있게 수정하기

private:
	void BeginRender();
	void EndRender();
};