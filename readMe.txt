### 종속성 설정

해당 프로젝트는 CMake로 엔진에 필요한 종속성 연결을 하고 있으며 해당 리포지토리에 들어있습니다.
External 폴더에서 확인 할 수 있습니다.

Imgui - docking
Directxtk
DirectTex
~~Assimp~~

### 파일 구조

Common
  프로젝트의 범용적으로 사용할만한 클래스를 모아두고 이를 정적라이브러리로 만들어서 Engine부에서 사용합니다.

Engine 
  Common 정적 라이브러리를 바탕으로 구현한 클래스들이 있고 에디터에 사용할 렌더 파이프 라인을 가지고 있습니다.
  에디터는 Engine 폴더를 빌드해 사용합니다.
  
