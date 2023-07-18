#pragma once
#include "../../Graphics/Shader.h"

class Game;

class DirLightShader : public Shader
{
public:
    DirLightShader(EngineCore* _engine);
    void Initialize(LPCWSTR shaderPath) override;
    
    
};
