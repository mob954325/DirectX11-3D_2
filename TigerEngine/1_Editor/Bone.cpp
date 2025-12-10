#include "Bone.h"

void Bone::CreateBone(string objName, int parentIndex, int boneIndex, Matrix worldMat, Matrix localMat)
{
	this->name = objName;
	this->parentIndex = parentIndex;
	this->index = boneIndex;

	this->worldTransform = worldMat;
	this->localTransform = localMat;
}