#pragma once
#include "../../Graphics/Shader.h"

class Game;

class OpaqueShader : public Shader
{
public:
    OpaqueShader();
    void Initialize(LPCWSTR shaderPath) override;

};
