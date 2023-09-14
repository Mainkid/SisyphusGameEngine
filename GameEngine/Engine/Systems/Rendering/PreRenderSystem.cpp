#include "PreRenderSystem.h"
#include "../../Core/EngineCore.h"
#include "../../Core/ServiceLocator.h"

void PreRenderSystem::Init()
{
    engine = EngineCore::instance();
    rc = ServiceLocator::instance()->Get<RenderContext>();
}

void PreRenderSystem::Run()
{
    
    engine->context->ClearRenderTargetView(engine->rtv.Get(), bgColor);
    engine->context->ClearRenderTargetView(rc->rtvs[0], bgColor);
    engine->context->ClearRenderTargetView(rc->rtvs[1], bgColor);
    engine->context->ClearRenderTargetView(rc->rtvs[2], bgColor);
    engine->context->ClearRenderTargetView(rc->rtvs[3], bgEntitiesColor);
    engine->context->ClearRenderTargetView(rc->rtvs[4], bgColor);
    engine->context->ClearDepthStencilView(engine->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    engine->context->RSSetViewports(1, &rc->viewport);
}

void PreRenderSystem::Destroy()
{
}
