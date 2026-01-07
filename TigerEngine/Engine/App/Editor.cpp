#include "Editor.h"
#include <System/ComponentFactory.h>
#include <commdlg.h>

// RTTR
#define RTTR_DLL
#include <rttr/registration>

void Editor::Render(std::unique_ptr<SceneSystem> &sceneSystem, HWND& hwnd)
{
    RenderMenuBar(sceneSystem, hwnd);
    RenderHierarchy(sceneSystem);
    RenderInspector();
}

void Editor::SelectObject(std::shared_ptr<GameObject> obj)
{
    selectedObject = obj;
}

void Editor::RenderMenuBar(std::unique_ptr<SceneSystem> &sceneSystem, HWND& hwnd)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save current scene"))
			{
				SaveCurrentScene(sceneSystem, hwnd);
			}
            else if(ImGui::MenuItem("Load scene"))
            {
                LoadScene(sceneSystem, hwnd);
            }

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
            
            if (ImGui::Selectable(obj->GetName().c_str(), selectedObject.lock() == obj))
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
        if(selectedObject.expired())
        {
            ImGui::Text("No gameObject selected");
        }
        else
        {
            auto obj = selectedObject.lock();
            /* ------------------------------- gameobject ------------------------------- */
            rttr::type t = rttr::type::get(obj.get());
            ImGui::Text("Type : %s", t.get_name().to_string().c_str());

            for(auto& prop : t.get_properties())
            {
                rttr::variant value = prop.get_value(obj);   // 프로퍼티 값
                std::string name = prop.get_name().to_string();         // 프로퍼티 이름
                if(value.is_type<std::string>() && name == "Name")
                {
                    ImGui::Text("Name : %s", name.c_str());
                    char buf[256]{};
                    strncpy_s(buf, value.get_value<std::string>().c_str(), sizeof(buf) - 1);
                    ImGui::InputText(name.c_str(), buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue);
                    prop.set_value(obj, std::string(buf));
                }
            }

            /* -------------------------------- transform ------------------------------- */
            if(ImGui::Button("Destory"))
            {
                obj->Destory();
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
                        creatorFunc(obj.get());
                    
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
            for(auto& comp : obj->GetIComponents())
            {                
                auto registeredComps = ComponentFactory::Instance().GetRegisteredComponents();
                auto name = comp->GetName();
                if(auto it = registeredComps.find(name); it != registeredComps.end())
                {
                    RenderComponentInfo(it->first, comp);
                    ImGui::NewLine();
                }
            }
        }
    }
    ImGui::End();
}

template<typename T>
void Editor::RenderComponentInfo(std::string compName, std::shared_ptr<T> comp)
{
    if(compName == "Transform")
    {
        rttr::type t = rttr::type::get(*comp); // 역참조로 실제 인스턴스 정보 가져오기
        ImGui::Text(t.get_name().to_string().c_str());

        for(auto& prop : t.get_properties())
        {
            rttr::variant value = prop.get_value(*comp);   // 프로퍼티 값
            std::string name = prop.get_name().to_string();         // 프로퍼티 이름
            if(value.is_type<DirectX::SimpleMath::Vector3>() && name == "Position")
            {
                DirectX::SimpleMath::Vector3 pos = value.get_value<DirectX::SimpleMath::Vector3>();
                ImGui::DragFloat3("Position", &pos.x, 0.1f);
                prop.set_value(*comp, pos);
            }
            else if(value.is_type<DirectX::SimpleMath::Vector3>() && name == "Rotation")
            {
                DirectX::SimpleMath::Vector3 rot = value.get_value<DirectX::SimpleMath::Vector3>();
                DirectX::SimpleMath::Vector3 rotEuler = { XMConvertToDegrees(rot.x), XMConvertToDegrees(rot.y),  XMConvertToDegrees(rot.z) };
                ImGui::DragFloat3("Rotation", &rotEuler.x, 0.1f);
                rot = { XMConvertToRadians(rotEuler.x), XMConvertToRadians(rotEuler.y),  XMConvertToRadians(rotEuler.z) };
                prop.set_value(*comp, rot);
            }
            else if(value.is_type<DirectX::SimpleMath::Vector3>() && name == "Scale")
            {
                DirectX::SimpleMath::Vector3 scl = value.get_value<DirectX::SimpleMath::Vector3>();
                ImGui::DragFloat3("Scale", &scl.x, 0.1f);
                prop.set_value(*comp, scl);
            }
        } 
    }
    else if(compName == "FBXData")
    {
        rttr::type t = rttr::type::get(*comp); // 역참조로 실제 인스턴스 정보 가져오기
        ImGui::Text(t.get_name().to_string().c_str());

        for(auto& prop : t.get_properties())
        {
            rttr::variant value = prop.get_value(*comp);   // 프로퍼티 값
            std::string name = prop.get_name().to_string();// 프로퍼티 이름
            if(value.is_type<std::string>() && name == "DataPath")
            {
                std::string path = value.get_value<std::string>();

                // 현재 경로 표시   
                ImGui::Text("Current Path: %s", path.c_str());
                
                // 탐색기 열기 버튼
                if (ImGui::Button("Browse..."))
                {
                    
                }
            }
        }        
    }
    else if(compName == "FBXRenderer")
    {
        ImGui::Text("FBXRenderer");
    }

    if (compName != "Transform") 
    {
        ImGui::PushID(comp.get());
        if(ImGui::Button("Remove Component"))
        {
            selectedObject.lock()->RemoveComponent(comp);
        }
        ImGui::PopID();
    }
}

void Editor::SaveCurrentScene(std::unique_ptr<SceneSystem>& sceneSystem, HWND& hwnd)
{
	// 파일 저장 다이얼로그
	OPENFILENAMEA ofn = {};
	char szFile[260] = {};

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = "json";

	if (GetSaveFileNameA(&ofn) != TRUE)
		return; // 사용자가 취소함

	std::string filename = szFile;

	// GameWorld를 파일에 저장
	if (sceneSystem->GetCurrentScene()->SaveToJson(filename))
	{
		MessageBoxA(hwnd, "Scene saved successfully!", "Save", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		MessageBoxA(hwnd, "Failed to save scene!", "Error", MB_OK | MB_ICONERROR);
	}
}

void Editor::LoadScene(std::unique_ptr<SceneSystem> &sceneSystem, HWND &hwnd)
{
    OPENFILENAMEA ofn ={};
    char szFile[256] = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT 
            | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = "json";

    // NOTE : GetOpenFileNameA를 한 뒤로 CWD (Current Working Directory)가 선택한 폴더로 변경된다.
    // ->  OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR 플래그 추가 해줘서 방지
    if (GetOpenFileNameA(&ofn) != TRUE) 
	    return; // 사용자가 취소함

    std::string filename = szFile;

    auto scene = sceneSystem->GetCurrentScene();

    // scene으로 파일 데이터 로드하기
    if (scene->LoadToJson(filename))
    {
    	MessageBoxA(hwnd, "Scene loaded successfully!", "Load", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
    	MessageBoxA(hwnd, "Failed to load scene! File not found.", "Error", MB_OK | MB_ICONERROR);
    }
}