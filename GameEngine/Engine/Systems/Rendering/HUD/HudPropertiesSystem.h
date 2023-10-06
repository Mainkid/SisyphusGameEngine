#pragma once
#include "../../ISystem.h"
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

struct EngineContext;
struct HardwareContext;
struct RenderContext;
class ResourceService;



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
    ResourceService* rs;
    std::string windowID;
    bool isFocused;
    
    void DrawMaterialProperties();
};
