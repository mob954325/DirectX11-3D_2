#include "Editor.h"

void Editor::Render(std::unique_ptr<SceneSystem> &sceneSystem)
{
    RenderHierarchy(sceneSystem);
}

// Hierarchy function
auto singleHierarchy(std::shared_ptr<GameObject> obj)
{
    ImGui::PushID(obj.get()); // 고유 아이디를 포인터로 설정 -> 나중에 UUID 만들꺼면 클래스를 따로두기, 로드할 때 ID 읽을 수 있게 만들어야함.

    ImGui::Text(obj->GetName().c_str());
    
    auto trans = obj->GetTransform();
    auto& pos = trans->position;
    auto& rot = trans->rotation;
    auto& scl = trans->scale;

    ImGui::DragFloat3("Position", &pos.x);
    ImGui::DragFloat3("Rotation", &rot.x);
    ImGui::DragFloat3("Scale", &scl.x);

    ImGui::NewLine();

    ImGui::PopID();
}

void Editor::RenderHierarchy(std::unique_ptr<SceneSystem> &sceneSystem)
{
    ImGui::Begin("Hierarchy");
    {
        sceneSystem->GetCurrentScene()->ForEachGameObject(singleHierarchy);
    }
    ImGui::End();
}