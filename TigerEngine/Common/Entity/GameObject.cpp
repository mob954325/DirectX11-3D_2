#include "GameObject.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<GameObject>("GameObject")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr) // ??
        .property("GameObject", &GameObject::name);
}

std::string GameObject::GetName() const
{
    return name;
}

void GameObject::SetName(std::string str)
{
    name = str;
}

void GameObject::RemoveComponent(std::weak_ptr<IComponent> comp)
{
    // 찾기
    for(auto it = components.begin(); it != components.end(); it++)
    {       
        if(*it == comp.lock())
        {
            components.erase(it);
            break;
        }
    }   
}

std::weak_ptr<Transform> GameObject::GetTransform() const
{
	return transform;
}

std::vector<std::shared_ptr<IComponent>> GameObject::GetIComponents()
{
    return components;
}

bool GameObject::IsDestory()
{
    return isDestory;
}

void GameObject::Destory()
{
	isDestory = true; // 가지고 있는 모든 컴포넌트 파괴하기
}

Scene *GameObject::GetScene()
{
    return currentScene;
}

void GameObject::SetScene(Scene* scene)
{
	transform = AddComponent<Transform>();
    currentScene = scene;
}

nlohmann::json GameObject::Serialize() const
{
    nlohmann::json datas;

    rttr::type t = rttr::type::get(*this);
    datas["type"] = t.get_name().to_string();       
    datas["properties"] = nlohmann::json::object(); // 객체 생성

    for(auto& prop : t.get_properties())
    {
        std::string propName = prop.get_name().to_string();
        rttr::variant value = prop.get_value(*this);

        if(value.is_type<std::string>())
        {
            datas["properties"][propName] = value.get_value<std::string>();
        }

        for(auto& comp : components)
        {
            //datas["properties"][propName] = comp->Serialize(); // TODO 컴포넌트 json 직렬화 완료하기
        }
    }
    
    return datas;
}
