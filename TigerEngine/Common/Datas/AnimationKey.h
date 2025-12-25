#pragma once
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class AnimationKey
{
public:
	float m_time;			// Ű ������ �ð�
	Vector3 m_position;		// �Ƹ� �ش� ���� ����� ��ġ�� ������ ����
	Quaternion m_rotation;	// 
	Vector3 m_scale;		// 
};
