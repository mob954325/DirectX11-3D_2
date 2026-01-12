#pragma once
#include "../pch.h"

/*
struct Vertex
{
    Vector3 position;
    Vector2 uv;
    Vector3 tangent;
    Vector3 biTangent;
    Vector3 normal;
};
*/

struct BoneWeightVertexData
{
    Vector3 position;
    Vector2 texture;
    Vector3 tangent;
    Vector3 bitangent;
    Vector3 normal;
	int BlendIndeces[4] = {};	// 
	float BlendWeights[4] = {};	// 

	void AddBoneData(int boneIndex, float weight)
	{
		assert(BlendWeights[0] == 0.0f ||
			BlendWeights[1] == 0.0f ||
			BlendWeights[2] == 0.0f ||
			BlendWeights[3] == 0.0f);

		for (int i = 0; i < 4; i++)
		{
			if (BlendWeights[i] == 0.0f)
			{
				BlendIndeces[i] = boneIndex;
				BlendWeights[i] = weight;
				return;
			}
		}
	}
};