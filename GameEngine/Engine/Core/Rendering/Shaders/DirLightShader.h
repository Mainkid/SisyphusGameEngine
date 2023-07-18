#pragma once
#include "../../Graphics/Shader.h"

class Game;

class DirLightShader : public Shader
{
public:
    DirLightShader();
    void Initialize(LPCWSTR shaderPath) override;
    
    
};
