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
