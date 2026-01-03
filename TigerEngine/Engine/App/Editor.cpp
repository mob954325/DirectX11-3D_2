#include "Editor.h"
#include <Manager/ComponentFactory.h>

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

    ImGui::DragFloat3("Position", &pos.x, 0.1f);
    ImGui::DragFloat3("Rotation", &rot.x, 0.1f);
    ImGui::DragFloat3("Scale", &scl.x, 0.1f);
    if(ImGui::Button("Destory"))
    {
        obj->Destory();
    }

    if(ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("ComponentMenu"); // 1. popup 열라고 명령 
        // open component menu
        // - select component -> ???
        // - call obj->AddComponent<T>()
    }

    // 2. 해당 ID를 가진 팝업이 열려있는지 확인하고 그림
    if (ImGui::BeginPopup("ComponentMenu")) 
    {                
        auto& componentsMap = ComponentFactory::Instance().GetRegisteredComponents();

        for(auto& [name, creatorFunc] : componentsMap)
        {
            // 컴포넌트 이름을 버튼 (MenuItem)으로 노출
            if(ImGui::MenuItem(name.c_str()))
            {
                // 1. 생성 람다 함수를 통해 새 컴포넌트 생성
                creatorFunc(obj);

                // 2. 현재 작업 중인 오브젝트에 추가
                // GameObject에 AddComponent(std::shared_ptr<Component>) 형태의 함수가 있어야 합니다.
                // obj->AddComponent(newComp); 
                ImGui::CloseCurrentPopup();
            }
        }


        ImGui::Separator();
        if (ImGui::MenuItem("Close")) { ImGui::CloseCurrentPopup(); }
    
        ImGui::EndPopup();
    }

    ImGui::NewLine();
    ImGui::PopID();
}

void Editor::RenderHierarchy(std::unique_ptr<SceneSystem> &sceneSystem)
{
    ImGui::Begin("Hierarchy");
    {
        if(ImGui::Button("Create GameObject"))
        {
            sceneSystem->GetCurrentScene()->AddGameObjectByName("NewGameObject");
        }

        sceneSystem->GetCurrentScene()->ForEachGameObject(singleHierarchy);
    }
    ImGui::End();
}