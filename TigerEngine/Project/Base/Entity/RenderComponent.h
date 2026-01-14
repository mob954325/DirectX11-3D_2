#pragma once
#include "../pch.h"
#include "../Entity/Component.h"
#include "../Renderer/IRenderCommand.h"

/// @brief 렌더링 처리하는 컴포넌트가 상속받는 컴포넌트
class RenderComponent : public Component
{
public:
    std::weak_ptr<IRenderCommand> GetCommand() { return command; }
    void SetCommand(std::shared_ptr<IRenderCommand> rc) { command = rc; }
protected:
    std::shared_ptr<IRenderCommand> command;
};