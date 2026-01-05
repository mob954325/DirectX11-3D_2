### 사용한 개발 환경

- OS : Window10 / 11 (x64, Win32)
- Compiler : MSVC v143
- Windows SDK : 10.0.26100.0
- Build System : CMake 
- CMake Version : 4.2+
- Package Manager : vcpkg

### 그래픽스 / 라이브러리 

- Graphics API : DirectX 11
- Libraries :
  - DirectX Tool Kit (DirectXTK) 	- 내부 External로 정적 라이브러리 생성
  - DirectXTex 				- 내부 External로 정적 라이브러리 생성
  - Imgui ( Docking )			- 내부 External로 정적 라이브러리 생성
  - Assimp 					- vcpkg로 관리

### 파일 구조

Common
  프로젝트의 범용적으로 사용할만한 클래스를 모아두고 이를 정적라이브러리로 만들어서 Engine부에서 사용합니다.

Engine 
  Common 정적 라이브러리를 바탕으로 구현한 클래스들이 있고 에디터에 사용할 렌더 파이프 라인을 가지고 있습니다.
  에디터는 Engine 폴더를 빌드해 사용합니다.
  
### 세팅하기

환경 변수 세팅
```
setx VCPKG_ROOT "설치된 vcpkg 경로"
```
위 코드 실행후 VSCode, 터미널 재시작 반드시 해야합니다.

```
$env:VCPKG_ROOT
```
로 실행해서 경로를 확인합니다.

종속성 세팅
```
vcpkg install nlohmann-json
vcpkg install assimp:x64-windows
```


