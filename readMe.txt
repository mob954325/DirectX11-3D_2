### 종속성 설정

위 레포는 vcpkg를 통해 라이브러리를 관리하고 있습니다.
필요한 라이브러리는 다음과같습니다.

Vcpkg - 마소에서 개발한 오픈 소스 라이브러리 관리자
https://github.com/microsoft/vcpkg.git

DirectX Tool kit - DX11 헬퍼 클래스 모음
https://github.com/microsoft/DirectXTK.git

DirectXTex - 텍스처 변환 및 최적화 용도
https://github.com/microsoft/DirectXTex

ImGUI - 실시간 값 체크 용
https://github.com/ocornut/imgui.git

Assimp - 3D 모델 파일 읽기
https://github.com/assimp/assimp.git

### vcpkg 경로 설정

.Props을 통해 vcpkg와 연결하고 있습니다. 

만약 vcpkg의 경로가 .Props와 다르다면 vcpkg 경로로 알맞게 변경해야합니다.

```jsx
<!-- vcpkg 루트 경로: 필요시 본인 경로로 수정 -->
<VcpkgRoot>D:\vcpkg</VcpkgRoot>
```

Vcpkg 설치하기 및 vcpkg로 소스 다운 

1. vcpkg git을 클론 후 bootstrap-vcpkg.bat 실행
2. 아래 코드를 vcpkg 폴더 경로를 가진 터미널에서 입력

```
vcpkg install directxtk:x64-windows-static-md
vcpkg install directxtex[dx11]:x64-windows-static-md
vcpkg install imgui[dx11-binding]:x64-windows-static-md
vcpkg install imgui[win32-binding]:x64-windows-static-md --recurse 
vcpkg install assimp:x64-windows        
```