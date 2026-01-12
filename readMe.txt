### 사용한 개발 환경

- OS : Windows 10 / 11 (x64)
- IDE : Visual Studio 2022
- Compiler : MSVC v143
- Windows SDK : 10.0.26100.0
- Build System : Visual Studio (MSBuild)
- Package Manager : vcpkg

### 그래픽스 / 라이브러리 

- Graphics API : DirectX 11
- Libraries :
  - DirectX Tool Kit (DirectXTK) 
  - DirectXTex 	
  - Imgui [Docking]
  - Assimp 
  - imguiFileDialog

### 파일 구조

Common
  프로젝트의 범용적으로 사용할만한 클래스를 모아두고 이를 정적라이브러리로 만들어서 Engine부에서 사용합니다.

Engine 
  Common 정적 라이브러리를 바탕으로 구현한 클래스들이 있고 에디터에 사용할 렌더 파이프 라인을 가지고 있습니다.
  에디터는 Engine 폴더를 빌드해 사용합니다.
  
### 종속성 세팅
```
vcpkg install nlohmann-json
vcpkg install assimp:x64-windows
vcpkg install imgui[docking-experimental]:x64-windows-static-md
vcpkg install imgui[win32-binding]:x64-windows-static-md --recurse
vcpkg install imgui[dx11-binding]:x64-windows-static-md --recurse
```


