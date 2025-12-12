#include "Transform.h"

Matrix Transform::GetWorldTransform() const
{
	return Matrix::CreateScale(scale) * 
		   Matrix::CreateFromYawPitchRoll(rotation) *
		   Matrix::CreateTranslation(position);
}
