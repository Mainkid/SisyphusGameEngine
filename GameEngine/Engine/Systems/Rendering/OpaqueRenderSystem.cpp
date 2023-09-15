#include "OpaqueRenderSystem.h"

#include "../EngineContext.h"
#include "../HardwareContext.h"
#include "RenderContext.h"
#include "../../Core/Graphics/ConstantBuffer.h"

void OpaqueRenderSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
    hc = ServiceLocator::instance()->Get<HardwareContext>();
}

void OpaqueRenderSystem::Run()
{
    hc->context->RSSetState(rc->cullBackRS.Get());
    auto view = ec->scene->registry.view<TransformComponent, MeshComponent>();
    for (auto& entity : view)
    {
        CB_BaseEditorBuffer dataOpaque;
        TransformComponent& transformComp = ec->scene->registry.get<TransformComponent>(entity);
        MeshComponent& meshComp = ec->scene->registry.get<MeshComponent>(entity);
        //dataOpaque.world = engineActor->transform->world * engineActor->transform->GetViewMatrix();
        dataOpaque.baseData.world = transformComp.transformMatrix;

        dataOpaque.baseData.worldViewProj =
            transformComp.transformMatrix *
            ec->scene->camera->view * ec->scene->camera->projection;

        dataOpaque.baseData.worldView = transformComp.transformMatrix *
            ec->scene->camera->view;

        dataOpaque.baseData.worldViewInverseTranspose =
            DirectX::XMMatrixTranspose(
                DirectX::XMMatrixInverse(nullptr,
                    transformComp.transformMatrix));

        dataOpaque.instanseID = (uint32_t)entity;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT res = hc->context->Map(rc->opaqueConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_BaseEditorBuffer));
        hc->context->Unmap(rc->opaqueConstBuffer->buffer.Get(), 0);
        hc->context->VSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());
        hc->context->PSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());

        for (int i = 0; i < meshComp.meshes.size(); i++)
        {
            hc->context->OMSetRenderTargets(5, rc->rtvs, hc->depthStencilView.Get());
            hc->context->IASetInputLayout(rc->opaqueShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            hc->context->IASetIndexBuffer(meshComp.meshes[i]->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, meshComp.meshes[i]->vertexBuffer->buffer.GetAddressOf(),
                meshComp.strides, meshComp.offsets);
            hc->context->PSSetSamplers(0, 1, meshComp.samplerState.GetAddressOf());
            hc->context->OMSetDepthStencilState(hc->depthStencilState.Get(), 0);
            hc->context->VSSetShader(rc->opaqueShader->vertexShader.Get(), nullptr, 0);
            hc->context->PSSetShader(rc->opaqueShader->pixelShader.Get(), nullptr, 0);
            hc->context->PSSetShaderResources(0, 1, meshComp.texture.GetAddressOf());
            hc->context->DrawIndexed(meshComp.meshes[i]->indexBuffer->size, 0, 0);
        }
    }
}

void OpaqueRenderSystem::Destroy()
{
}
