#include "ToneMappingRenderSystem.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../RenderContext.h"

SyResult ToneMappingRenderSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	hc = ServiceLocator::instance()->Get<HardwareContext>();

	SY_LOG_CORE(SY_LOGLEVEL_INFO, "ToneMappingSystem system initialization successful.");
	return SyResult();
}

SyResult ToneMappingRenderSystem::Run()
{
	UINT strides[1] = { 32 };
	UINT offsets[1] = { 0 };
	hc->context->PSSetSamplers(0, 1, rc->samplerState.GetAddressOf());
	hc->renderTarget->SetRenderTarget(hc->depthStencilView.Get());
	hc->context->PSSetShaderResources(0, 1, rc->gBuffer->HDRBufferSRV.GetAddressOf());
	hc->context->VSSetShader(rc->toneMapper->vertexShader.Get(), nullptr, 0);
	hc->context->PSSetShader(rc->toneMapper->pixelShader.Get(), nullptr, 0);
	hc->context->IASetInputLayout(rc->toneMapper->layout.Get());
	hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
	hc->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	hc->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
		strides, offsets);
	hc->context->DrawIndexed(6, 0, 0);

	return SyResult();

}

SyResult ToneMappingRenderSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "ToneMappingSystem system destroyed successful.");
	return SyResult();
}
