#pragma once
#include "../../ISystem.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class HudPostRenderSystem : public ISystem
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
