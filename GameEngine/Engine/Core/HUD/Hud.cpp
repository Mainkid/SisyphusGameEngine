#include "Hud.h"
#include "../EngineCore.h"

void Hud::Initialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }


    ImGui_ImplWin32_Init(EngineCore::instance()->window->GetHWND());
    ImGui_ImplDX11_Init(EngineCore::instance()->device.Get(), EngineCore::instance()->context.Get());

    widgets.insert(std::make_unique<ViewportWidget>());
    //ImGui::StyleColorsDark();
}

void Hud::Render()
{
   
    EngineCore::instance()->context->OMSetRenderTargets(1, EngineCore::instance()->rtv.GetAddressOf(), EngineCore::instance()->depthStencilView.Get());
    bool show = true;
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    
    for (auto& widget : widgets)
    {
        widget->Render();
    }
    //ImGui::ShowDemoWindow(&show);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}
