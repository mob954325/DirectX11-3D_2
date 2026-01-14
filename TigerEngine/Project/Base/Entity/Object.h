#pragma once
#include "../System/ObjectSystem.h"

/// <summary>
/// 엔진에서 사용하는 모든 객체가 상속받는 클래스
/// </summary>
class Object
{
public:
	Object() = default;
	virtual ~Object() = default;
};