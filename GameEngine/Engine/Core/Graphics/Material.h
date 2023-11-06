#pragma once

#include "SimpleMath.h"

class Material
{
public:
    Material(){};
    Material(DirectX::SimpleMath::Vector4 _coef)
    {
        specularCoef=_coef;
    }
    
    DirectX::SimpleMath::Vector4 specularCoef;
};
