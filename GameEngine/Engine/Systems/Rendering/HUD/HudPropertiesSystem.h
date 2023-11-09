#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "../../../../vendor/ImGui/curve_v122.hpp"
#include <string>

//#define _USE_MATH_DEFINES
#include <math.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/lexical_cast.hpp>
#include "../../../../vendor/entt/entt.hpp"
#include <utility>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "ImCurve.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;
class ResourceService;

//namespace ImGui
//{
//    int Curve(const char* label, const ImVec2& size, int maxpoints, ImVec2* points);
//    float CurveValue(float p, int maxpoints, const ImVec2* points);
//    float CurveValueSmooth(float p, int maxpoints, const ImVec2* points);
//};


class HudPropertiesSystem : public SystemBase
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;
private:
    EngineContext* ec;
    RenderContext* rc;
    HardwareContext* hc;
    ResourceService* rs;
    std::string windowID;
    boost::uuids::uuid prevSelectedUUID = boost::uuids::nil_uuid();
    bool isFocused;
    
    void DrawMaterialProperties();
    void UpdateHudProperties(bool);
};
