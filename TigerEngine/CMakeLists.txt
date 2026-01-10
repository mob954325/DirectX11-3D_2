cmake_minimum_required(VERSION 3.20) # CMake Version 사용

set(PROJECT_NAME GameEngine) 	# PROJECT_NAME
set(CMAKE_CXX_STANDARD 17)  		# CMAKE_ 붙은 변수는 전역변수다
project(${PROJECT_NAME}) 			# 해당 프로젝트를 세팅할 것이다.

# =========================================================
# 1. Common 라이브러리 정의
# =========================================================
# Common 파일 리스트 - 정적 라이브러리 만들기 위함
set(COMMON_SOURCE
    # root
    Common/GameApp.cpp
    Common/Helper.cpp
    Common/pch.cpp

    # data
    Common/Datas/Animation.cpp
    Common/Datas/Bone.cpp
    Common/Datas/Mesh.cpp
    Common/Datas/SkeletonInfo.cpp

    # Entity
    Common/Entity/Camera.cpp
    Common/Entity/GameObject.cpp
    Common/Entity/Transform.cpp

    # Renderer
    Common/Renderer/DirectX11Renderer.cpp
    Common/Renderer/ImguiRenderer.cpp

    # Scene
    Common/Scene/Scene.cpp

    # System
    Common/System/InputSystem.cpp
    Common/System/SceneSystem.cpp
    Common/System/TimeSystem.cpp
    Common/System/ComponentFactory.h
    Common/System/CameraSystem.cpp
)

# 라이브러리 추가
add_library(common_lib STATIC
    ${COMMON_SOURCE}
)

# common 라이브러리 헤더 경로 설정
# PUBLLIC으로 설정하면 이 라이브러리를 연결하는 타겟도 이 경로를 알게됨
target_include_directories(common_lib PUBLIC 
    common

    External/imgui
	External/imgui/backends/
    External/directxtex
	External/directxtk
)
# =========================================================
# 1. ImGui 라이브러리 정의
# =========================================================

# 1. ImGui를 정적 라이브러리 (Static Library)로 만듭니다.
add_library(imgui_lib STATIC
	External/imgui/imgui.cpp
	External/imgui/imgui_draw.cpp
	External/imgui/imgui_widgets.cpp
	External/imgui/imgui_tables.cpp
	External/imgui/imgui_demo.cpp
	# === 벡엔드 ===
	External/imgui/backends/imgui_impl_win32.cpp
	External/imgui/backends/imgui_impl_dx11.cpp
)

# 2. Imgui의 헤더 경로를 라이브러리에 연결합니다.
target_include_directories(imgui_lib PUBLIC
	External/imgui
	External/imgui/backends/
)

# 정적 라이브러리 빌드 시 보안 경고 무시 정의
if(MSVC)
    target_compile_definitions(imgui_lib PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# =========================================================
# 2. 메인 실행 파일 정의
# =========================================================

# 실행파일에 추가한다. 중간에 WIN32을 붙이면 윈도우창으로 실행됨
add_executable(${PROJECT_NAME} WIN32 
    # main
    Engine/winMain.cpp

    # App
    Engine/App/Editor.cpp
    Engine/App/EngineApp.cpp

    # RenderPass
    Engine/RenderPass/BasicRenderPass.cpp
    Engine/RenderPass/GBufferRenderPass.cpp
    Engine/RenderPass/DirectionalLightPass.cpp
    Engine/RenderPass/SkyboxRenderPass.cpp
    Engine/RenderPass/ShadowRenderPass.cpp

    # Components
    Engine/Components/FBXData.cpp
    Engine/Components/FBXRenderer.cpp
    Engine/Components/MeshRenderer.cpp

    # Manager
    Engine/Manager/FBXResourceManager.cpp
    Engine/Manager/ShaderManager.cpp

    # Commands
    Engine/Commands/DrawMeshCommand.cpp
    Engine/Commands/DrawFBXCommand.cpp
)

# Engine부에서 사용할 폴더들
target_include_directories(${PROJECT_NAME} PRIVATE
    Engine
)

if(MSVC)
	# 인코딩 경고(C4819) 해결 : UTF-8 컴파일 강제
	target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
	# 윈도우 환경에서 유니코드 처리를 기본으로 설정 (WinMain/TCHAR 등 사용시)
	target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()

# =========================================================
# 2. 외부 라이브러리 (소스 직접 컴파일) 정의
# =========================================================
# DirectXTK 폴더를 호출하여 그 안에 정의된 라이브러리 타겟(DirectXTK_Lib)을 가져옵니다.
add_subdirectory(External/directxtex)
add_subdirectory(External/directxtk)
add_subdirectory(External/ImguiFileDialog)

find_package(assimp CONFIG REQUIRED) # assimp/헤더파일명 으로 가져오기
find_package(nlohmann_json CONFIG REQUIRED) # assimp/헤더파일명 으로 가져오기
find_package(RTTR CONFIG REQUIRED)

# =========================================================
# 3. 라이브러리 연결 및 종속성 설정
# =========================================================
target_link_libraries(common_lib PUBLIC 
    assimp::assimp
    nlohmann_json::nlohmann_json
    RTTR::Core
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    # 1. 자체 정의 라이브러리
    common_lib
    imgui_lib

    # 2. External에 있는 라이브러리
    DirectXTK_Lib
    DirectXTex_Lib
    ImGuiFileDialog # Imgui 파일 탐색기 https://github.com/aiekick/ImGuiFileDialog/tree/master

    # 3. 시스템 라이브러리
    d3d11
    dxgi
    d3dcompiler 
    user32

    # vcpkg
    assimp::assimp
    nlohmann_json::nlohmann_json
    RTTR::Core
)

# =========================================================
# 4. 리소스 및 셰이더 파일 복사 설정
# =========================================================

# hlsl 셰이더 파일 추가
set(PS_FILES
    PS_Basic.hlsl
    PS_DepthOnlyPass.hlsl
    PS_DirectionalLight.hlsl
    PS_GBuffer.hlsl
    PS_PBR.hlsl
    PS_Skybox.hlsl
    PS_Solid.hlsl
)

set(VS_FILES
    VS_Basic.hlsl
    VS_DpethOnlyPass.hlsl
    VS_GBuffer.hlsl
    VS_SkinnedMesh.hlsl
    VS_Skybox.hlsl
)

# 셰이더 타입, 모델 설정
set_source_files_properties(
    ${PS_FILES}
    PROPERTIES
    VS_SHADER_TYPE "Pixel" # Set the shader stage (Vertex, Pixel, Compute, etc.)
    VS_SHADER_MODEL "5.0"  # Set the desired shader model (e.g., 5.0, 6.0, 6.6)
)

set_source_files_properties(
    ${VS_FILES}
    PROPERTIES
    VS_SHADER_TYPE "Vertex"
    VS_SHADER_MODEL "5.0"
)

# =========================================================
# 5. 리소스 및 셰이더 파일 복사 설정
# =========================================================

# 1. 복사할 리소스 폴더 설정 
set(RESOURCE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders")

# 2. 실행 파일 폴더로 복사하는 커멘드 추가
# POST_BUILD : 빌드 성공적으로 끝난 후 실행
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${RESOURCE_SOURCE_DIR}"        
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets"
    COMMENT "Copying resources to build directory..."
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SHADER_SOURCE_DIR}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders"
    COMMENT "Copying shaders to build directory..."
)