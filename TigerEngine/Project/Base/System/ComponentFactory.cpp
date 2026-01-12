#include "ComponentFactory.h"

using createCompFunc = std::function<std::weak_ptr<IComponent>(GameObject*)>;

const std::unordered_map<std::string, createCompFunc>& ComponentFactory::GetRegisteredComponents()
{
    return registeredComponents;
}