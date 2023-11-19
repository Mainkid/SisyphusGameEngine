#pragma once
#include "../Core/ECS/SystemBase.h"

class SyCollisionPreSystem : public SystemBase
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;
};
