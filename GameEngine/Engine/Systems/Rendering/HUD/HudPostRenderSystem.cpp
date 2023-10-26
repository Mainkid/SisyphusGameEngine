#include "HudPostRenderSystem.h"
#include "../../../Core/ServiceLocator.h"
#include "imgui_impl_dx11.h"
#include "../../../vendor/ImGui/imgui.h"
#include "../../HardwareContext.h"
#include "../../EngineContext.h"
#include "../../vendor/ImGui/imgui.h"

SyResult HudPostRenderSystem::Init()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    ec = ServiceLocator::instance()->Get<EngineContext>();
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "HudPostRender system initialization successful.");
    return SyResult();
}

SyResult HudPostRenderSystem::Run()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::ShowDemoWindow();
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

    SY_LOG_CORE(SY_LOGLEVEL_INFO, "HudPostRender system destruction successful.");
    return SyResult();
}


