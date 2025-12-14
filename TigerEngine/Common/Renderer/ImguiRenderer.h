#include "wrl/client.h"
#include "d3d11.h"

using namespace Microsoft::WRL;

class ImguiRenderer
{
private:

public:
    ImguiRenderer() = default;
    ~ImguiRenderer();

    void Initialize(HWND hwnd, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContex);
    void BeginRender();
    void Render();
    void EndRender();
};