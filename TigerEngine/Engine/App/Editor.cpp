#include "Editor.h"
#include <System/ComponentFactory.h>

// RTTR
#define RTTR_DLL
#include <rttr/registration>

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
            /* ------------------------------- gameobject ------------------------------- */
            rttr::type t = rttr::type::get(selectedObject.get());
            ImGui::Text("Type : %s", t.get_name().to_string().c_str());

            for(auto& prop : t.get_properties())
            {
                rttr::variant value = prop.get_value(selectedObject);   // 프로퍼티 값
                std::string name = prop.get_name().to_string();         // 프로퍼티 이름
                if(value.is_type<std::string>() && name == "GameObject")
                {
                    ImGui::Text("Name : %s", name.c_str());
                    char buf[256]{};
                    strncpy_s(buf, value.get_value<std::string>().c_str(), sizeof(buf) - 1);
                    ImGui::InputText(name.c_str(), buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue);
                    prop.set_value(selectedObject, std::string(buf));
                }
            }

            /* -------------------------------- transform ------------------------------- */
            auto trans = selectedObject->GetTransform();
            auto& pos = trans->position;
            auto& rot = trans->rotation;
            auto& scl = trans->scale;

            ImGui::DragFloat3("Position", &pos.x, 0.1f);

            Vector3 rotEuler = { XMConvertToDegrees(rot.x), XMConvertToDegrees(rot.y),  XMConvertToDegrees(rot.z) };
            ImGui::DragFloat3("Rotation", &rotEuler.x, 0.1f);
            rot = { XMConvertToRadians(rotEuler.x), XMConvertToRadians(rotEuler.y),  XMConvertToRadians(rotEuler.z) };

            ImGui::DragFloat3("Scale", &scl.x, 0.1f);
            if(ImGui::Button("Destory"))
            {
                selectedObject->Destory();
            }
            
            /* ---------------------------- add component 내용 ---------------------------- */
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

            /* ------------------------------- 컴포넌트 내용 출력 ------------------------------- */
            for(auto& comp : selectedObject->GetIComponents())
            {                
                // ...
                auto compsmap = ComponentFactory::Instance().GetRegisteredComponents();
                auto it = compsmap.find(std::string(typeid(comp).name())); // TODO 이름 넘길 방법 찾아야함 컴포넌트는 등록된 이름을 모름
                if(it != compsmap.end())
                {
                    // TODO 컴포넌트 별로 출력 내용 처리 260105
                    ImGui::Text(it->first.c_str());
                    ImGui::NewLine();
                }
            }
        }
        else
        {
            ImGui::Text("No gameObject selected");
        }
    }
    ImGui::End();
}
