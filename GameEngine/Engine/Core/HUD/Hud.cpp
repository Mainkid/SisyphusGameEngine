#include "Hud.h"

#include "../../../vendor/ImGui/imgui.h"
#include "../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../vendor/ImGui/imgui_impl_win32.h"
#include "../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../EngineCore.h"

void Hud::Initialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    io.WantCaptureKeyboard = false;
    io.WantCaptureMouse = true;
    
    
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    //io.AddKeyEvent

    ImGui_ImplWin32_Init(EngineCore::instance()->window->GetHWND());
    ImGui_ImplDX11_Init(EngineCore::instance()->device.Get(), EngineCore::instance()->context.Get());

    widgets.insert(std::make_unique<ViewportWidget>(this));
    widgets.insert(std::make_unique<HierarchyWidget>(this));
    widgets.insert(std::make_unique<ContentBrowser>(this));
    widgets.insert(std::make_unique<PropertiesWidget>(this));

    UpdateSelectedEntityEvent.AddRaw(this, &Hud::UpdateSelectedEntity);
    
}

void Hud::Render()
{
    
    
    EngineCore::instance()->context->OMSetRenderTargets(1, EngineCore::instance()->rtv.GetAddressOf(), EngineCore::instance()->depthStencilView.Get());
    bool show = true;
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    for (auto& widget : widgets)
    {
        widget->Render();
    }
    ImGui::ShowDemoWindow(&show);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void Hud::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

std::string Hud::GetFocusedWindowName()
{
    return focusedWindowName;
}

void Hud::SetFocusedWindowName(std::string _str)
{
    this->focusedWindowName = _str;
}

void Hud::UpdateSelectedEntity(entt::entity _id)
{
    this->selectedEntityID = _id;
}

