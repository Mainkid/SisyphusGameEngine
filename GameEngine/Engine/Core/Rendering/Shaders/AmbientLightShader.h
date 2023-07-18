#pragma once
#include "../../Graphics/Shader.h"

class AmbientLightShader :public Shader
{
public:
    AmbientLightShader(EngineCore* _game);
    void Initialize(LPCWSTR shaderPath) override;
    
};
