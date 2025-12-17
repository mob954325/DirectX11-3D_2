#pragma once
#include <Entity/IRenderComponent.h>
#include <Components/MeshFilter.h>

class MeshRenderer : public IRenderComponent
{
private:
	std::shared_ptr<MeshFilter> targetMeshFilter; // mesh data for rendering
public:
	void OnInitialize() override;
	void OnStart() override;
	void OnUpdate(float delta) override;
    void OnRender(std::unique_ptr<RenderQueue>& queue) override;
};