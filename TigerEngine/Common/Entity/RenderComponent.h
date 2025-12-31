#pragma once
#include <pch.h>
#include <Entity/IComponent.h>
#include <Renderer/IRenderCommand.h>

/// @brief 렌더링 처리하는 컴포넌트가 상속받는 컴포넌트
class RenderComponent : public IComponent
{
public:
    std::shared_ptr<IRenderCommand> GetCommand() { return command.lock(); }
    void SetCommand(std::weak_ptr<IRenderCommand> rc) { command = rc; }
protected:
    std::weak_ptr<IRenderCommand> command;
};