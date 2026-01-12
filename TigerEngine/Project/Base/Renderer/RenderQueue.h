#pragma once
#include "../pch.h"
#include "../Renderer/IRenderCommand.h"

class RenderQueue
{
private:
    std::vector<std::shared_ptr<IRenderCommand>> commands;
public:
    void AddCommand(std::shared_ptr<IRenderCommand> command)
    {
        commands.push_back(command);
    }

    const std::vector<std::shared_ptr<IRenderCommand>>& GetCommand() const
    {
        return commands;
    }

    void Clear()
    {
        commands.clear();
    }

    void Execute(ComPtr<ID3D11DeviceContext> context)
    {
        // execute command
        std::for_each(commands.begin(), commands.end(), [&context](auto comm)
        { 
            comm->Execute(context);
        });
    }

    // sort command for render order
    // void SortCommands();
};