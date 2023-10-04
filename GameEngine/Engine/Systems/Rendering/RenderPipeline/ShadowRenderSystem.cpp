#include "ShadowRenderSystem.h"
#include "../RenderContext.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../../Core/ServiceLocator.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../Components/LightComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/MeshComponent.h"
#include "DirectXMath.h"
#include "../../../Scene/CameraHelper.h"

void ShadowRenderSystem::Init()
{
    ec = ServiceLocator::instance()->Get<EngineContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    pointlightShadowProjectionMat = DirectX::XMMatrixPerspectiveFovLH(1.5708f, rc->SHADOWMAP_WIDTH * 1.0f / rc->SHADOWMAP_HEIGHT, 0.01f, 3.0f);
}

void ShadowRenderSystem::Run()
{
    hc->context->ClearDepthStencilView(rc->shadowStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    auto [camera, cameraTf] = CameraHelper::Find(_ecs);

    auto viewLights = _ecs->view<LightComponent, TransformComponent>();
    for (auto& lightEnt : viewLights)
    {
        auto [light, lightTf] = viewLights.get(lightEnt);
        if (light.lightType == LightType::Directional)
        {
            hc->context->RSSetState(rc->cullNoneRS.Get());
            hc->context->OMSetRenderTargets(1, &rc->m_renderTargetView, rc->shadowStencilView.Get());
            rc->m_renderTargetTexture = nullptr;
            auto viewMeshes = _ecs->view<TransformComponent, MeshComponent>();
            for (auto& ent : viewMeshes)
            {
                auto [transform, meshComp] = viewMeshes.get(ent);

                CB_ShadowBuffer dataShadow;
                //dataShadow.baseData.world = engineActor->transform->world * engineActor->transform->GetViewMatrix();
                dataShadow.baseData.world = transform.transformMatrix;

                dataShadow.baseData.worldViewProj = transform.transformMatrix * camera.view * camera.projection;

                dataShadow.baseData.worldView = transform.transformMatrix * camera.view;

                dataShadow.baseData.worldViewInverseTranspose =
		                XMMatrixTranspose(
			                XMMatrixInverse(nullptr,
			                                transform.transformMatrix * camera.view));

                for (int i = 0; i < 4; i++)
                    dataShadow.viewProjs[i] = light.viewMatrices[i] * light.orthoMatrices[i];

                D3D11_MAPPED_SUBRESOURCE mappedResource;
                HRESULT res = hc->context->Map(rc->shadowConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                CopyMemory(mappedResource.pData, &dataShadow, sizeof(CB_ShadowBuffer));
                hc->context->Unmap(rc->shadowConstBuffer->buffer.Get(), 0);
                hc->context->VSSetConstantBuffers(0, 1, rc->shadowConstBuffer->buffer.GetAddressOf());
                hc->context->PSSetConstantBuffers(0, 1, rc->shadowConstBuffer->buffer.GetAddressOf());
                hc->context->GSSetConstantBuffers(0, 1, rc->shadowConstBuffer->buffer.GetAddressOf());

                for (auto& mesh : meshComp.meshes)
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
                        meshComp.strides, meshComp.offsets);
                    hc->context->DrawInstanced(mesh->indexBuffer->size, 4, 0, 0);
                    
                }
            }
            hc->context->GSSetShader(nullptr, nullptr, 0);
        }
        else if (light.lightType == LightType::PointLight && (light.lightBehavior==LightBehavior::Movable ||
            (light.lightBehavior==LightBehavior::Static && light.shouldBakeShadows)))
        {
            light.shouldBakeShadows = false;
            hc->context->RSSetState(rc->cullNoneRS.Get());
            float bgColor[4] = { 0,0,0,1 };
            hc->context->ClearDepthStencilView(light.shadowMapDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
            hc->context->ClearRenderTargetView(light.shadowMapRTV.Get(),bgColor);
           
            CB_PointlightShadowBuffer dataShadow;
            dataShadow.world = Matrix::CreateTranslation(lightTf.position);
            
            //dataShadow.baseData.world = engineActor->transform->world * engineActor->transform->GetViewMatrix();
            dataShadow.view[0] = DirectX::XMMatrixLookAtLH( Vector3(lightTf.position), lightTf.position + Vector3(1, 0, 0),
                Vector3(0, 1, 0));
            dataShadow.view[1] = DirectX::XMMatrixLookAtLH(Vector3(lightTf.position), lightTf.position + Vector3(-1, 0, 0),
                Vector3(0, 1, 0));
            dataShadow.view[2] = DirectX::XMMatrixLookAtLH(Vector3(lightTf.position), lightTf.position + Vector3(0, 1, 0),
                Vector3(0, 0, -1));
            dataShadow.view[3] = DirectX::XMMatrixLookAtLH(Vector3(lightTf.position), lightTf.position + Vector3(0, -1, 0),
                Vector3(0, 0, 1));
            dataShadow.view[4] = DirectX::XMMatrixLookAtLH(Vector3(lightTf.position), lightTf.position + Vector3(0, 0, 1),
                Vector3(0, 1, 0));
            dataShadow.view[5] = DirectX::XMMatrixLookAtLH(Vector3(lightTf.position), lightTf.position + Vector3(0, 0, -1),
                Vector3(0, 1, 0));


            dataShadow.proj = pointlightShadowProjectionMat;
            
            auto viewMeshes = _ecs->view<TransformComponent, MeshComponent>();
            for (auto& ent : viewMeshes)
            {
                auto [transform, meshComp] = viewMeshes.get(ent);

                dataShadow.world = transform.transformMatrix;
                
                D3D11_MAPPED_SUBRESOURCE mappedResource;
                HRESULT res = hc->context->Map(rc->shadowPointlightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                CopyMemory(mappedResource.pData, &dataShadow, sizeof(CB_PointlightShadowBuffer));
                hc->context->Unmap(rc->shadowPointlightConstBuffer->buffer.Get(), 0);

                hc->context->VSSetConstantBuffers(0, 1, rc->shadowPointlightConstBuffer->buffer.GetAddressOf());
                hc->context->PSSetConstantBuffers(0, 1, rc->shadowPointlightConstBuffer->buffer.GetAddressOf());
                hc->context->GSSetConstantBuffers(0, 1, rc->shadowPointlightConstBuffer->buffer.GetAddressOf());


                D3D11_VIEWPORT viewport = {};
                viewport.Width = static_cast<float>(rc->SHADOWMAP_WIDTH);
                viewport.Height = static_cast<float>(rc->SHADOWMAP_HEIGHT);
                viewport.TopLeftX = 0;
                viewport.TopLeftY = 0;
                viewport.MinDepth = 0;
                viewport.MaxDepth = 1.0f;

                hc->context->RSSetViewports(1, &viewport);

                hc->context->IASetInputLayout(rc->shadowPointLightShader->layout.Get());
                hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                hc->context->OMSetRenderTargets(1, light.shadowMapRTV.GetAddressOf(), light.shadowMapDSV.Get());
                hc->context->PSSetShader(rc->shadowPointLightShader->pixelShader.Get(), nullptr, 0);
                hc->context->VSSetShader(rc->shadowPointLightShader->vertexShader.Get(), nullptr, 0);
                hc->context->GSSetShader(rc->shadowPointLightShader->geomShader.Get(), nullptr, 0);

                for (auto& mesh : meshComp.meshes)
                {
                    UINT offset[1] = { 0 };
                    UINT stride[1] = { 48 };


                    hc->context->IASetIndexBuffer(mesh->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                    //!hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                    //    engineActor->model->strides, engineActor->model->offsets);
                    hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                        meshComp.strides, meshComp.offsets);
                    hc->context->DrawInstanced(mesh->indexBuffer->size, 6, 0, 0);
                    
                }
               
            }
            hc->context->PSSetShader(nullptr, nullptr, 0);
            hc->context->GSSetShader(nullptr, nullptr, 0);
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
