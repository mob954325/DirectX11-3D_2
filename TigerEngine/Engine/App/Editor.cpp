#include "Editor.h"
#include <Manager/ComponentFactory.h>

void Editor::Render(std::unique_ptr<SceneSystem> &sceneSystem)
{
    RenderMenuBar();
    RenderHierarchy(sceneSystem);
    RenderInspector();
}

void Editor::SelectObject(std::shared_ptr<GameObject> obj)
{
    selectedObject = obj;
}

void Editor::RenderMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {

            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();
}

void Editor::RenderHierarchy(std::unique_ptr<SceneSystem> &sceneSystem)
{
    ImGui::Begin("World Hierarchy");
    {
        if(ImGui::Button("Create GameObject"))
        {
            sceneSystem->GetCurrentScene()->AddGameObjectByName("NewGameObject");
        }

        sceneSystem->GetCurrentScene()->ForEachGameObject([this](std::shared_ptr<GameObject> obj)
        {
            ImGui::PushID(obj.get()); // 고유 ID 부여 (ID 충돌 방지)
            
            if (ImGui::Selectable(obj->GetName().c_str(), selectedObject == obj))
            {
                this->SelectObject(obj);
            }

            ImGui::PopID();
        });
    }
    ImGui::End();
}

void Editor::RenderInspector()
{
    ImGui::Begin("Inspector");
    {
        if(selectedObject) 
        {
            auto trans = selectedObject->GetTransform();
            auto& pos = trans->position;
            auto& rot = trans->rotation;
            auto& scl = trans->scale;

            ImGui::DragFloat3("Position", &pos.x, 0.1f);
            ImGui::DragFloat3("Position", &pos.x, 0.1f);

            Vector3 rotEuler = { XMConvertToDegrees(rot.x), XMConvertToDegrees(rot.y),  XMConvertToDegrees(rot.z) };
            ImGui::DragFloat3("Rotation", &rotEuler.x, 0.1f);
            rot = { XMConvertToRadians(rotEuler.x), XMConvertToRadians(rotEuler.y),  XMConvertToRadians(rotEuler.z) };
            Vector3 rotEuler = { XMConvertToDegrees(rot.x), XMConvertToDegrees(rot.y),  XMConvertToDegrees(rot.z) };
            ImGui::DragFloat3("Rotation", &rotEuler.x, 0.1f);
            rot = { XMConvertToRadians(rotEuler.x), XMConvertToRadians(rotEuler.y),  XMConvertToRadians(rotEuler.z) };

            ImGui::DragFloat3("Scale", &scl.x, 0.1f);
            if(ImGui::Button("Destory"))
            {
                selectedObject->Destory();
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
                        creatorFunc(selectedObject);
                    
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
        }
            ImGui::DragFloat3("Scale", &scl.x, 0.1f);
            if(ImGui::Button("Destory"))
            {
                selectedObject->Destory();
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
                        creatorFunc(selectedObject);
                    
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
        }
    }
    ImGui::End();
}

