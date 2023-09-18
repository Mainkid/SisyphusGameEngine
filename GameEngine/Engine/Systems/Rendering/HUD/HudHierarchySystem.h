#pragma once
#include "../../ISystem.h"
#include "../../../vendor/ImGui/imgui.h"
#include "../../vendor/entt/entt.hpp"
#include <set>


struct EngineContext;
struct HardwareContext;
struct RenderContext;

class HudHierarchySystem : public ISystem
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
    
    const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    void RenderTree(std::set<entt::entity>& gameObjectsVector);
    
};
