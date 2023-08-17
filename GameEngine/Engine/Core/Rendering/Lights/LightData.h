#pragma once
#include "../../../../vendor/DirectXHelpers.h"
#include "../../../../vendor/SimpleMath.h"

using namespace DirectX::SimpleMath;
struct LightData
{
    Vector4 Pos;
    Vector4 Dir;
    Vector4 Color;
    Vector4 additiveParams;
};