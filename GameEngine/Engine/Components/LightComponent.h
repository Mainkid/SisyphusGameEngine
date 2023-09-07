#pragma once
#include "DirectXHelpers.h"
#include "SimpleMath.h"
#include "../Core/Rendering/Lights/LightType.h"
#include <memory>
#include <vector>

class EngineCore;


using namespace DirectX::SimpleMath;
class LightComponent
{
public:
    LightComponent() = default;
    LightComponent(LightType _type) 
    {
        lightType = _type;
    };
    std::vector<Matrix> viewMatrices;
    std::vector<Matrix> orthoMatrices;
    std::vector<Vector4> distances;
    std::unique_ptr<Mesh> aabb;
    LightType lightType = LightType::Ambient;
    Vector4 position = { 0,0,0,1 };
    Vector4 direction = { 1,0,0,0 };
    Vector4 color = { 1,1,1,1 };
    Vector4 params = {0,1,1,1 };
    uint32_t hash = 0;
    Matrix viewMatrix;
    Matrix orthoMatrix;
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
            result_type const h2(std::hash<Vector4>()(a.direction));
            result_type const h3(std::hash<Vector4>()(a.color));
            return h1 * 37 + (h1 * 37 + h2) * 37 + ((h1 * 37 + h2) * 37 + h3);
        }
    };
}

//TODO: Добавить MeshComponent aabb!!!