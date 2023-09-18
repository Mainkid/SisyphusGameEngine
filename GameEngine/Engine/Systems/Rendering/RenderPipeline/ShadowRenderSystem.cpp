#include "ShadowRenderSystem.h"
#include "../RenderContext.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../../Core/ServiceLocator.h"
#include "../../Core/Graphics/ConstantBuffer.h"

void ShadowRenderSystem::Init()
{
    ec = ServiceLocator::instance()->Get<EngineContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    hc = ServiceLocator::instance()->Get<HardwareContext>();
}

void ShadowRenderSystem::Run()
{
    hc->context->RSSetState(rc->cullBackRS.Get());
    hc->context->ClearDepthStencilView(rc->shadowStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    auto view = ec->scene->registry.view<LightComponent>();
    for (auto& entity : view)
    {
        LightComponent& light = view.get<LightComponent>(entity);
        if (light.lightType == LightType::Directional)
        {
            //TODO!!!! ������� ��� ������ � ������� �������, � �� ������� �����!!!
            /*light.GenerateViewMatrix(Vector3(engine->cameraController->camera->pos));
            light.GenerateOrthoFromFrustum(engine->cameraController->GetViewMatrix(), engine->cameraController->GetProjectionMatrix());
            light.GenerateOrthosFromFrustum(engine->cameraController->GetViewMatrix(), engine->cameraController->GetProjectionMatrix(), engine->cameraController->camera->farZ);*/


            hc->context->OMSetRenderTargets(1, &rc->m_renderTargetView, rc->shadowStencilView.Get());
            rc->m_renderTargetTexture = nullptr;
            auto view = ec->scene->registry.view<MeshComponent>();
            for (auto& go_entity : view)
            {
                TransformComponent& transform = ec->scene->registry.get<TransformComponent>(go_entity);
                MeshComponent& meshComponent = ec->scene->registry.get<MeshComponent>(go_entity);
                CB_ShadowBuffer dataShadow;
                //dataShadow.baseData.world = engineActor->transform->world * engineActor->transform->GetViewMatrix();
                dataShadow.baseData.world = transform.transformMatrix;

                dataShadow.baseData.worldViewProj =
                    transform.transformMatrix *
                    ec->scene->camera->view * ec->scene->camera->projection;

                dataShadow.baseData.worldView = transform.transformMatrix *
                    ec->scene->camera->view;

                dataShadow.baseData.worldViewInverseTranspose =
                    DirectX::XMMatrixTranspose(
                        DirectX::XMMatrixInverse(nullptr,
                            transform.transformMatrix * ec->scene->camera->view));

                for (int i = 0; i < 4; i++)
                    dataShadow.viewProjs[i] = light.viewMatrices[i] * light.orthoMatrices[i];

                D3D11_MAPPED_SUBRESOURCE mappedResource;
                HRESULT res = hc->context->Map(rc->shadowConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                CopyMemory(mappedResource.pData, &dataShadow, sizeof(CB_ShadowBuffer));
                hc->context->Unmap(rc->shadowConstBuffer->buffer.Get(), 0);
                hc->context->VSSetConstantBuffers(0, 1, rc->shadowConstBuffer->buffer.GetAddressOf());
                hc->context->PSSetConstantBuffers(0, 1, rc->shadowConstBuffer->buffer.GetAddressOf());
                hc->context->GSSetConstantBuffers(0, 1, rc->shadowConstBuffer->buffer.GetAddressOf());

                for (auto& mesh : meshComponent.meshes)
                {
                    UINT offset[1] = { 0 };
                    UINT stride[1] = { 48 };

                    D3D11_VIEWPORT viewport = {};
                    viewport.Width = static_cast<float>(rc->SHADOWMAP_WIDTH);
                    viewport.Height = static_cast<float>(rc->SHADOWMAP_HEIGHT);
                    viewport.TopLeftX = 0;
                    viewport.TopLeftY = 0;
                    viewport.MinDepth = 0;
                    viewport.MaxDepth = 1.0f;

                    hc->context->RSSetViewports(1, &viewport);

                    hc->context->IASetInputLayout(rc->shadowShader->layout.Get());
                    hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    hc->context->OMSetRenderTargets(1, &rc->m_renderTargetView, rc->shadowStencilView.Get());
                    hc->context->VSSetShader(rc->shadowShader->vertexShader.Get(), nullptr, 0);
                    hc->context->GSSetShader(rc->shadowShader->geomShader.Get(), nullptr, 0);

                    hc->context->IASetIndexBuffer(mesh->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                    //!hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                    //    engineActor->model->strides, engineActor->model->offsets);
                    hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                        meshComponent.strides, meshComponent.offsets);
                    hc->context->DrawInstanced(mesh->indexBuffer->size, 4, 0, 0);
                    hc->context->GSSetShader(nullptr, nullptr, 0);
                }
            }
        }
    }



    rc->viewport.Width = static_cast<float>(hc->window->GetWidth());
    rc->viewport.Height = static_cast<float>(hc->window->GetHeight());
    rc->viewport.TopLeftX = 0;
    rc->viewport.TopLeftY = 0;
    rc->viewport.MinDepth = 0;
    rc->viewport.MaxDepth = 1.0f;
    hc->context->RSSetViewports(1, &rc->viewport);
}

void ShadowRenderSystem::Destroy()
{

}
