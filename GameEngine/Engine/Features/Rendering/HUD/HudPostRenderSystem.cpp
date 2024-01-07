#include "HudPostRenderSystem.h"
#include "../../../Core/ServiceLocator.h"
#include "../../../vendor/ImGui/imgui.h"
#include "imgui_impl_dx11.h"

#include "../../../Contexts/HardwareContext.h"
#include "../../../Contexts/EngineContext.h"

SyResult HudPostRenderSystem::Init()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    ec = ServiceLocator::instance()->Get<EngineContext>();
    return SyResult();
}

SyResult HudPostRenderSystem::Run()
{
    OPTICK_EVENT();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //ImGui::ShowDemoWindow();
    ImGui::Render();
    
    

    hc->context->OMSetRenderTargets(1, hc->rtv.GetAddressOf(), nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    

    if ((io.ConfigFlags & ImGuiConfigFlags_DockingEnable) && !ec->isClosed) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    

    return SyResult();
}

SyResult HudPostRenderSystem::Destroy()
{
    return SyResult();
}


