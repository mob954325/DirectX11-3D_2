#pragma once
#include "pch.h"
#include <Renderer/IRenderCommand.h>

class RenderQueue
{
private:
    std::vector<std::unique_ptr<IRenderCommand>> commands;
public:
    void AddCommand(std::unique_ptr<IRenderCommand> command)
    {
        commands.push_back(std::move(command));
    }

    const std::vector<std::unique_ptr<IRenderCommand>>& GetCommand() const
    {
        return commands;
    }

    void Clear()
    {
        commands.clear();
    }

    // sort command for render order
    // void SortCommands();
};