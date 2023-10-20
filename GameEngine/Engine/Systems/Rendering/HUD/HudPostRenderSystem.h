#pragma once
#include "../../Core/ECS/SystemBase.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class HudPostRenderSystem : public SystemBase
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;
private:
    EngineContext* ec;
    RenderContext* rc;
    HardwareContext* hc;
};
