#include "../ClientGlobal.h"
#include "../Entity/GameObject.h"
#include "Player.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<Player>("Player")
        .constructor<>()(rttr::policy::ctor::as_std_shared_ptr);
}

void Player::Register()
{
    if (ENGINE_FACTORY)
    {
        ENGINE_FACTORY->Instance().Register<Player>("Player");
    }
}

void Player::OnInitialize()
{
}

void Player::OnStart()
{
}

void Player::OnUpdate(float delta)
{
    auto tran = owner->GetTransform();

    tran->position.x += delta;
}
