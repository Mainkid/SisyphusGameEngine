#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "../../../vendor/ImGui/imgui.h"
#include "../../../vendor/ImGui/imgui_internal.h"
#include "../../vendor/entt/entt.hpp"
#include <set>


struct EngineContext;
struct HardwareContext;
struct RenderContext;

class HudHierarchySystem : public SystemBase
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;
private:
    EngineContext* ec;
    RenderContext* rc;
    HardwareContext* hc;
    std::string windowID;
    bool isFocused;
    
    const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    void RenderTree(std::set<entt::entity> entities);
    
};
