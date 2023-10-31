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

SyResult ShadowRenderSystem::Init()
{
    ec = ServiceLocator::instance()->Get<EngineContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    hc = ServiceLocator::instance()->Get<HardwareContext>();
   
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "ShadowRender system initialization successful. ");
    return SyResult();
}

SyResult ShadowRenderSystem::Run()
{
    hc->context->ClearDepthStencilView(rc->ShadowStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    auto [camera, cameraTf] = CameraHelper::Find(_ecs);

    auto viewLights = _ecs->view<LightComponent, TransformComponent>();
    for (auto& lightEnt : viewLights)
    {
        auto [light, lightTf] = viewLights.get(lightEnt);
        if (light.LightType == LightType::Directional)
        {
            hc->context->RSSetState(rc->CullNoneRasterizerState.Get());
            hc->context->OMSetRenderTargets(1, &rc->MRenderTargetView, rc->ShadowStencilView.Get());
            //rc->MRenderTargetTexture = nullptr;
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
                    dataShadow.viewProjs[i] = light.ViewMatrices[i] * light.OrthoMatrices[i];

                D3D11_MAPPED_SUBRESOURCE mappedResource;
                HRESULT res = hc->context->Map(rc->ShadowConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                CopyMemory(mappedResource.pData, &dataShadow, sizeof(CB_ShadowBuffer));
                hc->context->Unmap(rc->ShadowConstBuffer->buffer.Get(), 0);
                hc->context->VSSetConstantBuffers(0, 1, rc->ShadowConstBuffer->buffer.GetAddressOf());
                hc->context->PSSetConstantBuffers(0, 1, rc->ShadowConstBuffer->buffer.GetAddressOf());
                hc->context->GSSetConstantBuffers(0, 1, rc->ShadowConstBuffer->buffer.GetAddressOf());

                for (auto& mesh : meshComp.model->meshes)
                {
                    UINT offset[1] = { 0 };
                    UINT stride[1] = { 48 };

                    D3D11_VIEWPORT viewport = {};
                    viewport.Width = static_cast<float>(rc->ShadowmapWidth);
                    viewport.Height = static_cast<float>(rc->ShadowmapHeight);
                    viewport.TopLeftX = 0;
                    viewport.TopLeftY = 0;
                    viewport.MinDepth = 0;
                    viewport.MaxDepth = 1.0f;

                    hc->context->RSSetViewports(1, &viewport);

                    hc->context->IASetInputLayout(rc->ShadowShader->layout.Get());
                    hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    hc->context->OMSetRenderTargets(1, &rc->MRenderTargetView, rc->ShadowStencilView.Get());
                    hc->context->VSSetShader(rc->ShadowShader->vertexShader.Get(), nullptr, 0);
                    hc->context->GSSetShader(rc->ShadowShader->geomShader.Get(), nullptr, 0);
                    hc->context->PSSetShader(rc->ShadowShader->pixelShader.Get(), nullptr, 0);
                    hc->context->IASetIndexBuffer(mesh->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                    //!hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                    //    engineActor->model->strides, engineActor->model->offsets);
                    hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                        meshComp.strides, meshComp.offsets);
                    hc->context->DrawIndexedInstanced(mesh->indexBuffer->size, 4, 0, 0,0);
                }
            }
            BlurShadowMap();
        }
        else if (light.LightType == LightType::PointLight && (light.LightBehavior==LightBehavior::Movable ||
            (light.LightBehavior==LightBehavior::Static && light.ShouldBakeShadows)))
        {
            pointlightShadowProjectionMat = DirectX::XMMatrixPerspectiveFovLH(1.5708f, rc->ShadowmapWidth * 1.0f / rc->ShadowmapHeight, 0.01f, light.ParamsRadiusAndAttenuation.x);

            light.ShouldBakeShadows = false;
            hc->context->RSSetState(rc->CullNoneRasterizerState.Get());
            float bgColor[4] = { 0,0,0,1 };
            hc->context->ClearDepthStencilView(light.ShadowCubeMapDsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
            hc->context->ClearRenderTargetView(light.ShadowCubeMapRtv.Get(),bgColor);
            

            CB_PointlightShadowBuffer dataShadow;
            dataShadow.world =Matrix::CreateScale(light.ParamsRadiusAndAttenuation.x, light.ParamsRadiusAndAttenuation.x,
                light.ParamsRadiusAndAttenuation.x)*Matrix::CreateTranslation(lightTf.position);
            
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
                HRESULT res = hc->context->Map(rc->ShadowPointlightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                CopyMemory(mappedResource.pData, &dataShadow, sizeof(CB_PointlightShadowBuffer));
                hc->context->Unmap(rc->ShadowPointlightConstBuffer->buffer.Get(), 0);

                hc->context->VSSetConstantBuffers(0, 1, rc->ShadowPointlightConstBuffer->buffer.GetAddressOf());
                hc->context->PSSetConstantBuffers(0, 1, rc->ShadowPointlightConstBuffer->buffer.GetAddressOf());
                hc->context->GSSetConstantBuffers(0, 1, rc->ShadowPointlightConstBuffer->buffer.GetAddressOf());


                D3D11_VIEWPORT viewport = {};
                viewport.Width = static_cast<float>(rc->ShadowmapWidth);
                viewport.Height = static_cast<float>(rc->ShadowmapHeight);
                viewport.TopLeftX = 0;
                viewport.TopLeftY = 0;
                viewport.MinDepth = 0;
                viewport.MaxDepth = 1.0f;

                hc->context->RSSetViewports(1, &viewport);

                hc->context->IASetInputLayout(rc->ShadowPointLightShader->layout.Get());
                hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                hc->context->OMSetRenderTargets(1, light.ShadowCubeMapRtv.GetAddressOf(), light.ShadowCubeMapDsv.Get());
                hc->context->PSSetShader(rc->ShadowPointLightShader->pixelShader.Get(), nullptr, 0);
                hc->context->VSSetShader(rc->ShadowPointLightShader->vertexShader.Get(), nullptr, 0);
                hc->context->GSSetShader(rc->ShadowPointLightShader->geomShader.Get(), nullptr, 0);

                for (auto& mesh : meshComp.model->meshes)
                {
                    UINT offset[1] = { 0 };
                    UINT stride[1] = { 48 };
                    hc->context->IASetIndexBuffer(mesh->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                    //!hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                    //    engineActor->model->strides, engineActor->model->offsets);
                    hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                        meshComp.strides, meshComp.offsets);
                    hc->context->DrawIndexedInstanced(mesh->indexBuffer->size, 6, 0, 0,0);
                    
                }
               
            }
            hc->context->PSSetShader(nullptr, nullptr, 0);
            hc->context->GSSetShader(nullptr, nullptr, 0);
        }
    }

    hc->context->GSSetShader(nullptr, nullptr, 0);

    rc->Viewport.Width = static_cast<float>(hc->window->GetWidth());
    rc->Viewport.Height = static_cast<float>(hc->window->GetHeight());
    rc->Viewport.TopLeftX = 0;
    rc->Viewport.TopLeftY = 0;
    rc->Viewport.MinDepth = 0;
    rc->Viewport.MaxDepth = 1.0f;
    hc->context->RSSetViewports(1, &rc->Viewport);

    return SyResult();
}

SyResult ShadowRenderSystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "ShadowRender system destruction successful. ");
    return SyResult();
}

void ShadowRenderSystem::BlurShadowMap()
{
    UINT strides[1] = { 32 };
    UINT offsets[1] = { 0 };

    hc->context->OMSetDepthStencilState(rc->OffStencilState.Get(), 0);
    hc->context->VSSetShader(rc->GaussianBlurX->vertexShader.Get(), nullptr, 0);
    hc->context->PSSetShader(rc->GaussianBlurX->pixelShader.Get(), nullptr, 0);
    hc->context->GSSetShader(rc->GaussianBlurX->geomShader.Get(), nullptr, 0);
    hc->context->PSSetSamplers(0, 1, rc->PointSampler.GetAddressOf());
    hc->context->IASetInputLayout(rc->GaussianBlurX->layout.Get());
    hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    hc->context->OMSetRenderTargets(1,&rc->MBluredShadowRtv , nullptr);

    hc->context->PSSetShaderResources(0, 1, &rc->MShaderResourceView);
    hc->context->IASetIndexBuffer(rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    hc->context->IASetVertexBuffers(0, 1, rc->VertexQuadBuffer->buffer.GetAddressOf(),
        strides, offsets);

    hc->context->DrawIndexedInstanced(6, 4, 0, 0, 0);

    hc->context->CopyResource(rc->MRenderTargetTexture, rc->MBluredShadowTexture);

    hc->context->PSSetShader(rc->GaussianBlurY->pixelShader.Get(), nullptr, 0);
    hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    hc->context->OMSetRenderTargets(1, &rc->MBluredShadowRtv, nullptr);
    hc->context->DrawIndexedInstanced(6, 4, 0, 0, 0);

    hc->context->OMSetDepthStencilState(hc->depthStencilState.Get(), 0);
}
