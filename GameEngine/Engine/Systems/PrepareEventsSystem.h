#pragma once
#include "../Core/ECS/SystemBase.h"
struct SyPrepareEventsSystem : public SystemBase
{
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;
};


