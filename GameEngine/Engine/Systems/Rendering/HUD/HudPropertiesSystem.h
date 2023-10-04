#pragma once
#include "../../Core/ECS/SystemBase.h"
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class HudPropertiesSystem : public SystemBase
{
public:
    void Init() override;
    void Run() override;
    void Destroy() override;
private:
    EngineContext* ec;
    RenderContext* rc;
    HardwareContext* hc;
    std::string windowID;
    bool isFocused;
    
    
};
