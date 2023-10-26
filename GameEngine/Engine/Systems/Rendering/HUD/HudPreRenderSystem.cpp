#include "HudPreRenderSystem.h"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../../vendor/ImGui/imgui_impl_win32.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../../../Systems/HardwareContext.h"
#include "../../../Core/ServiceLocator.h"
#include "../../../Tools/MathHelper.h"

SyResult HudPreRenderSystem::Init()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    /*io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;*/
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.WantCaptureKeyboard = false;
    io.WantCaptureMouse = false;


    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    //io.AddKeyEvent

    ImGui_ImplWin32_Init(hc->window->GetHWND());
    ImGui_ImplDX11_Init(hc->device.Get(), hc->context.Get());
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "HudPreRender system initialization successful.");
    return SyResult();
}

SyResult HudPreRenderSystem::Run()
{
    hc->context->OMSetRenderTargets(1, hc->rtv.GetAddressOf(), nullptr);
    bool show = true;
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    if (windowHeight != io.DisplaySize.y || windowWidth != io.DisplaySize.x)
    {
        CleanupRenderTarget();
    	windowWidth  = SyMathHelper::Max(static_cast<int>(io.DisplaySize.x), 0);
        windowHeight = SyMathHelper::Max(static_cast<int>(io.DisplaySize.y), 0);
        hc->swapChain->ResizeBuffers(0, windowWidth, windowHeight, DXGI_FORMAT_UNKNOWN, 0);
        CreateRenderTarget();
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());



    /*for (auto& widget : widgets)
    {
        widget->Render();
    }*/

    //ImGui::ShowDemoWindow(&show);

    //ImGui::Render();



    //hc->context->OMSetRenderTargets(1, hc->rtv.GetAddressOf(), nullptr);
    //ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


    //if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    //    ImGui::UpdatePlatformWindows();
    //    ImGui::RenderPlatformWindowsDefault();
    //}
    return SyResult();
}

SyResult HudPreRenderSystem::Destroy()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    SY_LOG_CORE(SY_LOGLEVEL_INFO, "HudPreRender system destruction successful.");
    return SyResult();
}

void HudPreRenderSystem::CleanupRenderTarget()
{
    if (hc->rtv.Get())
    {
        hc->rtv = nullptr;
    }
}

void HudPreRenderSystem::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    hc->swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    hc->device->CreateRenderTargetView(pBackBuffer, nullptr, hc->rtv.GetAddressOf());
    pBackBuffer->Release();
}

