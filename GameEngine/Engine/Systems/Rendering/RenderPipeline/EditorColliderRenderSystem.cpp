#include "EditorColliderRenderSystem.h"
#include "SimpleMath.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../../Rendering/RenderContext.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../../../Scene/CameraHelper.h"

SyResult EditorColliderRenderSystem::Init()
{
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	_rc = ServiceLocator::instance()->Get<RenderContext>();
	_hc = ServiceLocator::instance()->Get<HardwareContext>();
	return SyResult();
}

SyResult EditorColliderRenderSystem::Run()
{
	CB_GridEditorBuffer dataOpaque;

	_hc->context->OMSetBlendState(_rc->GridBlendState.Get(), nullptr, 0xffffffff);
	auto [camera, cameraTransform] = CameraHelper::Find(_ecs);

	dataOpaque.baseData.worldViewProj =
		camera.view * camera.projection;
	_hc->context->OMSetDepthStencilState(_rc->OffStencilState.Get(), 0);



	D3D11_MAPPED_SUBRESOURCE mappedResource;

	HRESULT res = _hc->context->Map(_rc->ShadowConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0,
		&mappedResource);
	CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_GridEditorBuffer));
	_hc->context->Unmap(_rc->ShadowConstBuffer->buffer.Get(), 0);
	_hc->context->VSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());
	_hc->context->PSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());
	_hc->renderTarget->SetRenderTarget(_hc->depthStencilView.Get());
	_hc->context->IASetInputLayout(_rc->EditorColliderRenderer->layout.Get());
	_hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	//_hc->context->IASetIndexBuffer(_grid1M->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_hc->context->IASetVertexBuffers(0, 1, _grid1M->vertexBuffer->buffer.GetAddressOf(),
		_rc->RhData.strides12, _rc->RhData.offsets0);


	_hc->context->VSSetShader(_rc->EditorColliderRenderer->vertexShader.Get(), nullptr, 0);
	_hc->context->PSSetShader(_rc->EditorColliderRenderer->pixelShader.Get(), nullptr, 0);
	_hc->context->Draw(_grid10M->indexBuffer->size, 0, 0);

	return SyResult();
}

SyResult EditorColliderRenderSystem::Destroy()
{

	return SyResult();
}




