#pragma once

enum class LightType
{
    Ambient,
    Directional,
    PointLight,
    SpotLight
};

enum class LightBehavior
{
    Static,
    Movable
};