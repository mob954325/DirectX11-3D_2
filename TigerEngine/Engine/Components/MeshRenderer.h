#pragma once
#include <Components/FBXData.h>
#include <Entity/RenderComponent.h>

class MeshRenderer : public RenderComponent
{
private:
	std::shared_ptr<FBXData> targetMeshData; // mesh data for rendering
	
public:
	void OnInitialize() override;
	void OnStart() override;
	void OnUpdate(float delta) override;
};