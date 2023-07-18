#pragma once
#include "../../Graphics/Shader.h"

class Game;

class OpaqueShader : public Shader
{
public:
    OpaqueShader(EngineCore* _engine);
    void Initialize(LPCWSTR shaderPath) override;

};
