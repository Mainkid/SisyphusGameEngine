#pragma once
#include "../../Graphics/Shader.h"

class AmbientLightShader :public Shader
{
public:
    AmbientLightShader();
    void Initialize(LPCWSTR shaderPath) override;
    
};
