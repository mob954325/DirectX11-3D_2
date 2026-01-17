#pragma once
#include <pch.h>
#include <System/Singleton.h>

/// @brief 셰이더에 사용하는 데이터를 가지고 있는 클래스
/// @date 26.01.03
class ShaderManager : public Singleton<ShaderManager>
{
public:
    ShaderManager(token) {};
    ~ShaderManager() = default;

    void CreateCB(const ComPtr<ID3D11Device>& dev);
    
    ComPtr<ID3D11Buffer>& GetCameraCB();
    ComPtr<ID3D11Buffer>& GetMaterialCB();
    ComPtr<ID3D11Buffer>& GetTransformCB();
    ComPtr<ID3D11Buffer>& GetBonePoseCB();
    ComPtr<ID3D11Buffer>& GetBoneOffsetCB();
    ComPtr<ID3D11Buffer>& GetDirectionalLightCB();

private:
    /* ---------------------------- constant Buffers ---------------------------- */
    ComPtr<ID3D11Buffer> cameraCB;
    ComPtr<ID3D11Buffer> materialCB;
    ComPtr<ID3D11Buffer> transformCB;
    ComPtr<ID3D11Buffer> bonePoseCB;
    ComPtr<ID3D11Buffer> boneOffsetCB;
    ComPtr<ID3D11Buffer> directionalLightCB;
};