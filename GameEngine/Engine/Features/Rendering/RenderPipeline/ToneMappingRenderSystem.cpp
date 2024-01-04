#include "ToneMappingRenderSystem.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../../Contexts/RenderContext.h"

SyResult ToneMappingRenderSystem::Init()
{
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	_rc = ServiceLocator::instance()->Get<RenderContext>();
	_hc = ServiceLocator::instance()->Get<HardwareContext>();

	return SyResult();
}

SyResult ToneMappingRenderSystem::Run()
{
	OPTICK_EVENT();
	_hc->context->PSSetSamplers(0, 1, _rc->SamplerState.GetAddressOf());
	_hc->renderTarget->SetRenderTarget(nullptr);
	_hc->context->PSSetShaderResources(0, 1, _rc->GBuffer->HdrBufferSrv.GetAddressOf());
	_hc->context->VSSetShader(_rc->ToneMapper->vertexShader.Get(), nullptr, 0);
	_hc->context->PSSetShader(_rc->ToneMapper->pixelShader.Get(), nullptr, 0);
	_hc->context->IASetInputLayout(_rc->ToneMapper->layout.Get());
	_hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
	_hc->context->IASetIndexBuffer(_rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_hc->context->IASetVertexBuffers(0, 1, _rc->VertexQuadBuffer->buffer.GetAddressOf(),
		_rc->RhData.strides32, _rc->RhData.offsets0);
	_hc->context->DrawIndexed(6, 0, 0);

	return SyResult();

}

SyResult ToneMappingRenderSystem::Destroy()
{
	return SyResult();
}
