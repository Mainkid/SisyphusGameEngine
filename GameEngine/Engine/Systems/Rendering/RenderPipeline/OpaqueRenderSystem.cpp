#include "OpaqueRenderSystem.h"

#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../RenderContext.h"
#include "../../../Scene/CameraHelper.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/MeshComponent.h"


SyResult OpaqueRenderSystem::Init()
{
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	_rc = ServiceLocator::instance()->Get<RenderContext>();
    _hc = ServiceLocator::instance()->Get<HardwareContext>();
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "OpaqueRender system initialization successful.");
    return SyResult();
}

SyResult OpaqueRenderSystem::Run()
{
    _hc->context->RSSetState(_rc->CullBackRasterizerState.Get());

    auto view =_ecs->view<TransformComponent, MeshComponent>();
    auto [camera, cameraTransform] = CameraHelper::Find(_ecs);
   
    for (auto& entity : view)
    {
        CB_BaseEditorBuffer dataOpaque;
        TransformComponent& transformComp = _ecs->get<TransformComponent>(entity);
        MeshComponent& meshComp = _ecs->get<MeshComponent>(entity);
        
        dataOpaque.baseData.world = transformComp.transformMatrix;

        dataOpaque.baseData.view =camera.view;

        dataOpaque.baseData.worldViewProj =
            transformComp.transformMatrix *
            camera.view * camera.projection;

        dataOpaque.baseData.worldView = transformComp.transformMatrix * camera.view;

        dataOpaque.baseData.worldViewInverseTranspose =
            DirectX::XMMatrixTranspose(
                DirectX::XMMatrixInverse(nullptr,
                    transformComp.transformMatrix));

        dataOpaque.instanseID.x = (uint32_t)entity;
        dataOpaque.instanseID.z = (uint32_t)EAssetType::ASSET_MESH;

        for (int i = 0; i < meshComp.model->meshes.size(); i++)
        {
            
            dataOpaque.materialData.albedo = meshComp.materials[i]->albedoValue;
            dataOpaque.materialData.emissive = meshComp.materials[i]->emissiveValue;
            dataOpaque.materialData.metallic = meshComp.materials[i]->metallicValue;
            dataOpaque.materialData.roughness = meshComp.materials[i]->roughnessValue;
            dataOpaque.materialData.specular = meshComp.materials[i]->specularValue;
            dataOpaque.instanseID.y = i;

            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT res = _hc->context->Map(_rc->OpaqueConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_BaseEditorBuffer));
            _hc->context->Unmap(_rc->OpaqueConstBuffer->buffer.Get(), 0);
            _hc->context->VSSetConstantBuffers(0, 1, _rc->OpaqueConstBuffer->buffer.GetAddressOf());
            _hc->context->PSSetConstantBuffers(0, 1, _rc->OpaqueConstBuffer->buffer.GetAddressOf());

            _hc->context->OMSetRenderTargets(7, _rc->Rtvs, _hc->depthStencilView.Get());
            _hc->context->IASetInputLayout(_rc->OpaqueShader->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            _hc->context->IASetIndexBuffer(meshComp.model->meshes[i]->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _hc->context->IASetVertexBuffers(0, 1, meshComp.model->meshes[i]->vertexBuffer->buffer.GetAddressOf(),
                meshComp.strides, meshComp.offsets);
            _hc->context->PSSetSamplers(0, 1, meshComp.materials[i]->albedoSRV->textureSamplerState.GetAddressOf());
            _hc->context->OMSetDepthStencilState(_hc->depthStencilState.Get(), 0);
            _hc->context->VSSetShader(_rc->OpaqueShader->vertexShader.Get(), nullptr, 0);
            _hc->context->PSSetShader(_rc->OpaqueShader->pixelShader.Get(), nullptr, 0);
            _hc->context->PSSetShaderResources(0, 6, meshComp.materials[i]->resources);
            _hc->context->DrawIndexed(meshComp.model->meshes[i]->indexBuffer->size, 0, 0);
        }
    }
    return SyResult();
}

SyResult OpaqueRenderSystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "OpaqueRender system destruction successful.");
    return SyResult();
}
