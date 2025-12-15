#include <Entity/IComponent.h>

class MeshFilter : public IComponent
{
public:
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;
};