#pragma once
#include "DirectXHelpers.h"
#include "SimpleMath.h"
#include <d3d11.h>
#include "../Components/Mesh.h"
#include "../Core/Rendering/Lights/LightType.h"
#include <memory>
#include <vector>
#include "../Serialization/Serializable.h"

class EngineCore;


using namespace DirectX::SimpleMath;
struct LightComponent
{
    LightComponent() = default;
    LightComponent(LightType _type)
    {
        lightType = _type;
    };
    std::vector<Matrix> viewMatrices;
    std::vector<Matrix> orthoMatrices;
    std::vector<Vector4> distances;
    std::shared_ptr<Mesh> aabb = nullptr;
    LightType lightType = LightType::Ambient;
    LightBehavior lightBehavior = LightBehavior::Movable;
    Vector4 color = { 1,1,1,1 };
    Vector4 paramsRadiusAndAttenuation = { 0,1,1,1 };
    uint32_t hash = 0;
    Matrix viewMatrix;
    Matrix orthoMatrix;
    bool shouldBakeShadows = true;

    //Rendering shadows
    Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowMapTexture = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilViewTexture = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowMapSRV = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> shadowMapRTV = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowMapDSV = nullptr;


    SER_COMP(LightComponent,
        lightType,
        lightBehavior,
        color,
        paramsRadiusAndAttenuation)
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
            result_type const h1(std::hash<Vector4>()(a.color));
            result_type const h3(std::hash<Vector4>()(a.paramsRadiusAndAttenuation));
            return h1 * 37 + (h1 * 37 + h3) * 37 + ((h1 * 37 + h3) * 37 + h3);
        }
    };
}

//TODO: Добавить MeshComponent aabb!!!