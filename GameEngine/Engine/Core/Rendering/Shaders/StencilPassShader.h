#pragma once

#include "../../Graphics/Shader.h"

class EngineCore;

class StencilPassShader : public Shader
{
public:
    StencilPassShader();
    void Initialize(LPCWSTR shaderPath) override;

    UINT strides[1]={48};
    UINT offsets[1]={0};
};
