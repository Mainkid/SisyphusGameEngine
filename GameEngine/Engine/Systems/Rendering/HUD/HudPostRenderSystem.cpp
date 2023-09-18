#include "HudPostRenderSystem.h"
#include "../../../Core/ServiceLocator.h"
#include "imgui_impl_dx11.h"
#include "../../../vendor/ImGui/imgui.h"
#include "../../HardwareContext.h"
#include "../../vendor/ImGui/imgui.h"

void HudPostRenderSystem::Init()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
}

void HudPostRenderSystem::Run()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::ShowDemoWindow();
    ImGui::Render();
    
    

    hc->context->OMSetRenderTargets(1, hc->rtv.GetAddressOf(), nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void HudPostRenderSystem::Destroy()
{
    
}


