#pragma once
#include "../../Graphics/Shader.h"


class SpotLightShader : public Shader
{
public:
    SpotLightShader();
    void Initialize(LPCWSTR shaderPath) override;

    UINT strides[1]={32};
    UINT offsets[1]={0};
};
