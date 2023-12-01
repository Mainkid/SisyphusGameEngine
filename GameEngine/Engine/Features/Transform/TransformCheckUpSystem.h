#pragma once

#include "../../Core/ECS/SystemBase.h"

class SyTransformCheckUpsystem : public SystemBase
{
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;
};

