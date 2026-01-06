#include "Transform.h"
#include "System/ComponentFactory.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<Transform>("Transform")
		.constructor<>()
			(rttr::policy::ctor::as_std_shared_ptr)
		.property("Position", &Transform::position)
		.property("Rotation", &Transform::rotation)
		.property("Scale", &Transform::scale);

	rttr::registration::class_<DirectX::SimpleMath::Vector3>("Vector3")
		.constructor<>()
		.constructor<float, float, float>()
		.property("x", &Vector3::x)
		.property("y", &Vector3::y)
		.property("z", &Vector3::z);
}

REGISTER_COMPONENT(Transform);

Matrix Transform::GetWorldTransform() const
{
	return Matrix::CreateScale(scale) * 
		   Matrix::CreateFromYawPitchRoll(rotation) *
		   Matrix::CreateTranslation(position);
}

nlohmann::json Transform::Serialize()
{
	nlohmann::json datas;

    rttr::type t = rttr::type::get(*this);
    datas["type"] = t.get_name().to_string();       
    datas["properties"] = nlohmann::json::object(); // 객체 생성

    for(auto& prop : t.get_properties())
    {
        std::string propName = prop.get_name().to_string();
        rttr::variant value = prop.get_value(*this);
	    if(value.is_type<DirectX::SimpleMath::Vector3>() && propName == "Position")
        {
            auto v = value.get_value<Vector3>();
            datas["properties"][propName] = {v.x, v.y, v.z};
        }
        else if(value.is_type<DirectX::SimpleMath::Vector3>() && propName == "Rotation")
        {
            auto v = value.get_value<Vector3>();
            datas["properties"][propName] = {v.x, v.y, v.z};
        }
        else if(value.is_type<DirectX::SimpleMath::Vector3>() && propName == "Scale")
        {
            auto v = value.get_value<Vector3>();
            datas["properties"][propName] = {v.x, v.y, v.z};
        }
	}

    return datas;
}
