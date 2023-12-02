#pragma once
#include "DirectXHelpers.h"
#include "SimpleMath.h"
#include <d3d11.h>
#include "../../Mesh/Components/MeshComponent.h"
#include "../../../Core/Rendering/Lights/ELightType.h"
#include <memory>
#include <vector>
#include "../../../Serialization/Serializable.h"

class EngineCore;


using namespace DirectX::SimpleMath;
struct LightComponent
{
    LightComponent() = default;
    LightComponent(ELightType _type)
    {
        LightType = _type;
    };

    //----User vars----
    ELightType LightType = ELightType::Ambient;
    LightBehavior LightBehavior = LightBehavior::Movable;
    Vector4 Color = { 1,1,1,1 };
    Vector4 ParamsRadiusAndAttenuation = { 0,1,1,1 };
    bool CastShadows = false;

    //----Engine vars----

    std::vector<Matrix> ViewMatrices;
    std::vector<Matrix> OrthoMatrices;
    std::vector<Vector4> Distances;
    std::shared_ptr<Mesh> Aabb = nullptr;
   
    uint32_t Hash = 0;
    Matrix ViewMatrix;
    Matrix OrthoMatrix;
    bool ShouldBakeShadows = true;

    int ShadowMapSize = 1024;

    size_t MonoHash = 0; // read/write only by mono sync system

    /*
     *  Rendering pointlight/spotlight shadows
     */
    Microsoft::WRL::ComPtr<ID3D11Texture2D> ShadowCubeMapTexture = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilViewCubeTexture = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShadowCubeMapSrv = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ShadowCubeMapRtv = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> ShadowCubeMapDsv = nullptr;

    /*
     *  Rendering directional cascade shadows
     */

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DirShadowStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DirShadowStencilView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> ShadowPointLightStencilView;
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> ShadowResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> ShadowMapSampler;

    Microsoft::WRL::ComPtr <ID3D11Texture2D> DirShadowTexture = nullptr;
    Microsoft::WRL::ComPtr <ID3D11Texture2D> DirShadowRtTexture;
    Microsoft::WRL::ComPtr <ID3D11Texture2D> DirBluredShadowTexture;
    Microsoft::WRL::ComPtr <ID3D11RenderTargetView> DirShadowRtv;
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> DirShadowSrv;
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> DirShadowBluredSrv;
    Microsoft::WRL::ComPtr <ID3D11RenderTargetView> DirShadowBluredRtv;



    SER_COMP(LightComponent,
        LightType,
        LightBehavior,
        Color,
        ParamsRadiusAndAttenuation)
};

//inline size_t hash_value(const Vector4& v)
//{
//    size_t hash = 0;
//    boost::hash_combine(hash, v.x);
//    boost::hash_combine(hash, v.y);
//    boost::hash_combine(hash, v.z);
//    boost::hash_combine(hash, v.w);
//    return hash;
//}
//
//inline size_t hash_value(const LightComponent& light)
//{
//    size_t hash = 0;
//    boost::hash_combine(hash, light.Color);
//    boost::hash_combine(hash, light.ParamsRadiusAndAttenuation);
//    return hash;
//}

//TODO: Добавить MeshComponent Aabb!!!