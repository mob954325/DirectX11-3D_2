#include "GameObject.h"
#include "DirectXCollision.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<GameObject>("GameObject")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr) 
        .property("Name", &GameObject::name);
}

 std::weak_ptr<IComponent> GameObject::AddComponentByName(std::string name, GameObject * obj)
{
	auto& comps = ComponentFactory::Instance().GetRegisteredComponents();

	for(auto& it : comps)
	{
		auto [compName, createFunc] = it;
		if(compName != name) continue;
		else
		{
			return createFunc(obj);
		}
	}

	return std::weak_ptr<IComponent>();
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
    currentScene = scene;
}

nlohmann::json GameObject::Serialize() const
{
    // gameObject
    //  properties
    //      name : gmaeName
    //      components 
    //          component_1
    //              component1_element_1
    //              component1_element_2
    //              ....
    //          component_2
    //              ....

    nlohmann::json datas;

    rttr::type t = rttr::type::get(*this);
    datas["type"] = t.get_name().to_string();    
    datas["properties"] = nlohmann::json::object(); // 객체 생성

    // 오브젝트 내용 직렬화화
    for(auto& prop : t.get_properties())
    {
        std::string propName = prop.get_name().to_string();
        rttr::variant value = prop.get_value(*this);

        if(value.is_type<std::string>())
        {
            datas["properties"][propName] = value.get_value<std::string>();
        }
    }

    // 컴포넌트 내용 직렬화
    datas["properties"]["components"] = nlohmann::json::object();
    auto& comps = datas["properties"]["components"];

    for (auto& comp : components)
    {
        comps[comp->GetName()] = comp->Serialize();
    }
    
    return datas;
}

void GameObject::Deserialize(const nlohmann::json objData)
{
    // objData : data["objects"]["properties"]

    rttr::type t = rttr::type::get(*this);
    if(!objData.contains("components")) return;

    for(auto& prop : objData["components"])
    {
        if(!prop.contains("type")) continue;

        std::string compName = prop["type"];
        if(compName == "Transform") // Transform은 게임 오브젝트가 생성 시에 추가된다.
        {
            auto weak = this->GetComponent<Transform>();
            weak.lock()->Deserialize(prop);
        }
        else    // 그 외 컴포넌트는 추가한다.
        {
            auto weak = AddComponentByName(compName, this);
        
            if(!weak.expired())
            {
                weak.lock()->Deserialize(prop);
            }
        }
    }
}

void GameObject::UpdateAABB()
{
    BoundingBox aabb({0.0f,0.0f,0.0f}, {2.5f,2.5f,2.5f});

    Transform* trans = transform.lock().get();

    //Vector3 updatedExtent = aabb.Extents * trans->scale;
    //aabbBox.Center = trans->position;
    //aabbBox.Extents = updatedExtent;

    Matrix worldMatrix = trans->GetWorldTransform();
    aabbBox.Transform(aabbBox, worldMatrix);
}

void GameObject::Initialize()
{
    transform = AddComponent<Transform>();
}