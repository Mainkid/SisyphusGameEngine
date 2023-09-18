#pragma once
#include "../../ISystem.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class HudPostRenderSystem : public ISystem
{
public:
    void Init() override;
    void Run() override;
    void Destroy() override;
private:
    EngineContext* ec;
    RenderContext* rc;
    HardwareContext* hc;
};
