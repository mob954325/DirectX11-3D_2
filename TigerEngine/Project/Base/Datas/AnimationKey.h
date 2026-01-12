#pragma once
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;

class AnimationKey
{
public:
	float m_time;			//
	Vector3 m_position;		//
	Quaternion m_rotation;	// 
	Vector3 m_scale;		// 
};
