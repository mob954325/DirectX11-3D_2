#include <Entity/IRenderComponent.h>

class MeshRenderer : public IRenderComponent
{
public:
	void OnInitialize() override;
	void OnStart() override;
	void OnUpdate(float delta) override;
    void OnRender(std::unique_ptr<RenderQueue>& queue) override;
};