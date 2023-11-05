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
	hc->context->PSSetSamplers(0, 1, rc->SamplerState.GetAddressOf());
	hc->renderTarget->SetRenderTarget(nullptr);
	hc->context->PSSetShaderResources(0, 1, rc->GBuffer->HdrBufferSrv.GetAddressOf());
	hc->context->VSSetShader(rc->ToneMapper->vertexShader.Get(), nullptr, 0);
	hc->context->PSSetShader(rc->ToneMapper->pixelShader.Get(), nullptr, 0);
	hc->context->IASetInputLayout(rc->ToneMapper->layout.Get());
	hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
	hc->context->IASetIndexBuffer(rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	hc->context->IASetVertexBuffers(0, 1, rc->VertexQuadBuffer->buffer.GetAddressOf(),
		strides, offsets);
	hc->context->DrawIndexed(6, 0, 0);

	return SyResult();

}

SyResult ToneMappingRenderSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "ToneMappingSystem system destroyed successful.");
	return SyResult();
}
