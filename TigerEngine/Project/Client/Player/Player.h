#pragma once
#include "../Entity/Component.h"

class Player : public Component
{
    RTTR_ENABLE(Component)
public:
    void Register();

    /// <summary>
    /// Component가 처음 실행될 때 실행됩니다.
    /// </summary>
    void OnInitialize() override;

    /// <summary>
    /// OnUpdate()를 실행하기 전 ***한 번*** 실행됩니다.
    /// </summary>
    void OnStart() override;

    /// <summary>
    /// GameEngine에 매 프레임마다 OnUpdate()내 에서 호출됩니다.
    /// </summary>
    void OnUpdate(float delta) override;
};

