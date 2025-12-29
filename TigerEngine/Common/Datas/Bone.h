#pragma once
#include "pch.h"
#include <string>
#include <SimpleMath.h>
#include "BoneAnimation.h"

using namespace std;

class Bone
{
public:
	void CreateBone(string objName, int parentIndex, int boneIndex, Matrix worldMat, Matrix localMat);

	BoneAnimation m_boneAnimation;	// 

	// private: -> ���Ǹ� ���� public ����
	Matrix m_worldTransform;
	Matrix m_localTransform;

	string name = "";		// �ش� bone�� �̸�

	int m_parentIndex = -1; // ���� ���������� �θ� �ε���
	int m_index = -1;		// 
};