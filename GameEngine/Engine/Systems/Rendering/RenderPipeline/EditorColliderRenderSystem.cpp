#include "EditorColliderRenderSystem.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../../Rendering/RenderContext.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../../../Components/ColliderComponent.h"
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
	if (_ec->playModeState == EngineContext::EPlayModeState::PlayMode && !_ec->Properties.DrawInPlayMode)
		return SyResult();

	CB_GridEditorBuffer dataOpaque;

	_hc->context->OMSetBlendState(_rc->GridBlendState.Get(), nullptr, 0xffffffff);
	auto [camera, cameraTransform] = CameraHelper::Find(_ecs);

	dataOpaque.baseData.worldViewProj = camera.view * camera.projection;
	_hc->context->OMSetDepthStencilState(_rc->OffStencilState.Get(), 0);



	

	auto view = _ecs->view<SyPrimitiveColliderComponent>();
	for (auto& ent:view)
	{
		if (_ec->Properties.DrawOnlySelected && !_ec->hudData.selectedEntityIDs.contains(ent))
			continue;

		auto& transform = _ecs->get<TransformComponent>(ent);
		auto& collider = _ecs->get<SyPrimitiveColliderComponent>(ent);
			if (collider._colliderGeometry.GetIndices().size()==0)
			continue;

			dataOpaque.baseData.world =Matrix::CreateScale(Vector3(1.001f,1.001f,1.001f))* Matrix::CreateFromYawPitchRoll(transform.localRotation) * Matrix::CreateTranslation(transform.localPosition);
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
		_hc->context->IASetIndexBuffer(collider._colliderGeometry.GetIndexBuffer()->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		_hc->context->IASetVertexBuffers(0, 1, collider._colliderGeometry.GetVertexBuffer()->buffer.GetAddressOf(),
			_rc->RhData.strides16, _rc->RhData.offsets0);


		_hc->context->VSSetShader(_rc->EditorColliderRenderer->vertexShader.Get(), nullptr, 0);
		_hc->context->PSSetShader(_rc->EditorColliderRenderer->pixelShader.Get(), nullptr, 0);
		_hc->context->DrawIndexed(collider._colliderGeometry.GetIndexBuffer()->size, 0, 0);
	}




	return SyResult();
}

SyResult EditorColliderRenderSystem::Destroy()
{

	return SyResult();
}
