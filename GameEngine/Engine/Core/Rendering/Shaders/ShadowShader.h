#pragma once

#include "../../Graphics/Shader.h"

class ShadowShader : public Shader
{
public:

    ShadowShader(EngineCore* _engine);
    void Initialize(LPCWSTR shaderPath) override;

};
