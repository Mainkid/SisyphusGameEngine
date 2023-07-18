#pragma once
#include "../../Graphics/Shader.h"

class EngineCore;

class PointLightShader : public Shader
{
public:
    PointLightShader(EngineCore* _engine);
    void Initialize(LPCWSTR shaderPath) override;

    UINT strides[1]={32};
    UINT offsets[1]={0};
};
