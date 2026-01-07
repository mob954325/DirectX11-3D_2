#pragma once
#include <pch.h>
#include <System/Singleton.h>
#include <Entity/GameObject.h>

// Component 등록 매크로 함수
#define REGISTER_COMPONENT(Type) \
    static bool Type##_registered = []() { \
        ComponentFactory::Instance().Register<Type>(#Type); \
        return true; \
    }()

/// @brief 컴포넌트 조회용 클래스
/// 엔진에 사용하는 모든 컴포넌트들은 해당 클래스에 등록된다.
class ComponentFactory : public Singleton<ComponentFactory>
{
public:
    ComponentFactory(token) {};
    ~ComponentFactory() = default;

    using createCompFunc = std::function<std::weak_ptr<IComponent>(GameObject*)>;

    template<typename T>
    void Register(std::string compName) 
    {
        auto createComp = [compName](GameObject* obj)
        { 
            auto comp = obj->AddComponent<T>();
            comp.lock()->SetName(compName);
            return comp; 
        };
        registeredComponents.insert({compName, createComp});
    }

    const std::unordered_map<std::string, createCompFunc>& GetRegisteredComponents()
    {
        return registeredComponents;
    }

private:
    std::unordered_map<std::string, createCompFunc> registeredComponents; // 컴포넌트 이름, 컴포넌트 생성 람다 함수
};