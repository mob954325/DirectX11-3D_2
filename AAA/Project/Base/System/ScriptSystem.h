#pragma once
#include "Singleton.h"
#include "../Entity/Component.h"

/// <summary>
/// 렌더링을 하지 않는 단순 컴포넌트를 상속받은 컴포넌트들을 관리합니다.
/// 객체의 생명주기는 관리하지 않습니다.
/// </summary>
class ScriptSystem : public Singleton<ScriptSystem>
{
public:
    ScriptSystem(token) {}
    ~ScriptSystem() = default;

    /// <summary>
    /// 시스템에 컴포넌트 등록
    /// </summary>
    void Register(Component* comp);

    /// <summary>
    /// 해당 컴포넌트 등록 해제
    /// </summary>
    void UnRegister(Component* comp);

    /// <summary>
    /// 등록된 컴포넌트 호출
    /// </summary>
    void Update(float delta);

private:
    /// <summary>
    /// 등록된 컴포넌드 목록
    /// </summary>
    std::vector<Component*> comps{};
};
