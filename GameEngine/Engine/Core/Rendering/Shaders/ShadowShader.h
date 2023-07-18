#pragma once

#include "../../Graphics/Shader.h"

class ShadowShader : public Shader
{
public:

    ShadowShader();
    void Initialize(LPCWSTR shaderPath) override;

};
