#include "PreRenderSystem.h"
#include "RenderContext.h"
#include "../EngineContext.h"
#include "../HardwareContext.h"
#include "../../Core/ServiceLocator.h"

SyResult PreRenderSystem::Init()
{
    rc = ServiceLocator::instance()->Get<RenderContext>();
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    ec = ServiceLocator::instance()->Get<EngineContext>();
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "PreRender system initialization successful.");
    return SyResult();
}

SyResult PreRenderSystem::Run()
{
    
    hc->context->ClearRenderTargetView(hc->rtv.Get(), bgColor);
    hc->context->ClearRenderTargetView(rc->rtvs[0], bgColor);
    hc->context->ClearRenderTargetView(rc->rtvs[1], bgColor);
    hc->context->ClearRenderTargetView(rc->rtvs[2], bgColor);
    hc->context->ClearRenderTargetView(rc->rtvs[3], bgColor);
    hc->context->ClearRenderTargetView(rc->rtvs[4], bgColor);
    hc->context->ClearRenderTargetView(rc->rtvs[5], bgColor);
    hc->context->ClearRenderTargetView(rc->rtvs[6], bgColor);
    hc->context->ClearRenderTargetView(rc->rtvs[7], bgEntitiesColor);
    hc->context->ClearRenderTargetView(rc->m_renderTargetView, bgColor);

    hc->context->ClearDepthStencilView(hc->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    hc->context->RSSetViewports(1, &rc->viewport);
    return SyResult();
}

SyResult PreRenderSystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "PreRender system destruction successful.");
    return SyResult();
}
