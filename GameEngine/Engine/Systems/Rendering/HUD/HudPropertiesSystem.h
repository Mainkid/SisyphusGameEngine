#pragma once
#include "../../ISystem.h"
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

struct EngineContext;
struct HardwareContext;
struct RenderContext;

enum class EShowMode
{
    ContentProperties,
    GameObjectProperties
};

class HudPropertiesSystem : public ISystem
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
    EShowMode showMode = EShowMode::GameObjectProperties;
    
    
};
