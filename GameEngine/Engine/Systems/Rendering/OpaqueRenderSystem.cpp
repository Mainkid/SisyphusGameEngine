#include "OpaqueRenderSystem.h"
#include "../../Core/EngineCore.h"
#include "../../Core/Graphics/ConstantBuffer.h"

void OpaqueRenderSystem::Init()
{
	engine = EngineCore::instance();
	rc = ServiceLocator::instance()->Get<RenderContext>();
}

void OpaqueRenderSystem::Run()
{
    engine->context->RSSetState(rc->cullBackRS.Get());
    auto view = engine->scene->registry.view<TransformComponent, MeshComponent>();
    for (auto& entity : view)
    {
        CB_BaseEditorBuffer dataOpaque;
        TransformComponent& transformComp = engine->scene->registry.get<TransformComponent>(entity);
        MeshComponent& meshComp = engine->scene->registry.get<MeshComponent>(entity);
        //dataOpaque.world = engineActor->transform->world * engineActor->transform->GetViewMatrix();
        dataOpaque.baseData.world = transformComp.transformMatrix;

        dataOpaque.baseData.worldViewProj =
            transformComp.transformMatrix *
            engine->scene->camera->view * engine->scene->camera->projection;

        dataOpaque.baseData.worldView = transformComp.transformMatrix *
            engine->scene->camera->view;

        dataOpaque.baseData.worldViewInverseTranspose =
            DirectX::XMMatrixTranspose(
                DirectX::XMMatrixInverse(nullptr,
                    transformComp.transformMatrix));

        dataOpaque.instanseID = (uint32_t)entity;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT res = engine->context->Map(rc->opaqueConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_BaseEditorBuffer));
        engine->context->Unmap(rc->opaqueConstBuffer->buffer.Get(), 0);
        engine->context->VSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());
        engine->context->PSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());

        for (int i = 0; i < meshComp.meshes.size(); i++)
        {
            engine->context->OMSetRenderTargets(5, rc->rtvs, engine->depthStencilView.Get());
            engine->context->IASetInputLayout(rc->opaqueShader->layout.Get());
            engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            engine->context->IASetIndexBuffer(meshComp.meshes[i]->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            engine->context->IASetVertexBuffers(0, 1, meshComp.meshes[i]->vertexBuffer->buffer.GetAddressOf(),
                meshComp.strides, meshComp.offsets);
            engine->context->PSSetSamplers(0, 1, meshComp.samplerState.GetAddressOf());
            engine->context->OMSetDepthStencilState(engine->depthStencilState.Get(), 0);
            engine->context->VSSetShader(rc->opaqueShader->vertexShader.Get(), nullptr, 0);
            engine->context->PSSetShader(rc->opaqueShader->pixelShader.Get(), nullptr, 0);
            engine->context->PSSetShaderResources(0, 1, meshComp.texture.GetAddressOf());
            engine->context->DrawIndexed(meshComp.meshes[i]->indexBuffer->size, 0, 0);
        }
    }
}

void OpaqueRenderSystem::Destroy()
{
}
