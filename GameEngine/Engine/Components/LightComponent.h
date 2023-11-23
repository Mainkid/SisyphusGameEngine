#pragma once
#include "DirectXHelpers.h"
#include "SimpleMath.h"
#include <d3d11.h>
#include "../Components/Mesh.h"
#include "../Core/Rendering/Lights/ELightType.h"
#include <memory>
#include <vector>
#include "../Serialization/Serializable.h"

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

namespace std
{
    template<> struct hash<Vector4>
    {
        using argument_type = Vector4;
        using result_type = std::size_t;
        result_type operator()(argument_type const& a) const
        {
            result_type const h1(std::hash<float>()(a.x));
            result_type const h2(std::hash<float>()(a.y));
            result_type const h3(std::hash<float>()(a.z));
            result_type const h4(std::hash<float>()(a.w));
            return h1*37+
                (h1*37+h2)*37+
                ((h1 * 37 + h2) * 37 + h3) * 37 + 
                (((h1 * 37 +h2) * 37 + h3) * 37)*h4;
        }
    };
}

namespace std
{
    template<> struct hash<LightComponent>
    {
        using argument_type = LightComponent;
        using result_type = std::size_t;
        result_type operator()(argument_type const& a) const
        {
            result_type const h1(std::hash<Vector4>()(a.Color));
            result_type const h3(std::hash<Vector4>()(a.ParamsRadiusAndAttenuation));
            return h1 * 37 + (h1 * 37 + h3) * 37 + ((h1 * 37 + h3) * 37 + h3);
        }
    };
}

//TODO: Добавить MeshComponent Aabb!!!