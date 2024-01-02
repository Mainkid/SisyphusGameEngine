#include "ShadowRenderSystem.h"
#include "../../../Contexts/RenderContext.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../Core/ServiceLocator.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../Lighting/Components/LightComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Mesh/Components/MeshComponent.h"
#include "DirectXMath.h"
#include "../../../Scene/CameraHelper.h"

SyResult ShadowRenderSystem::Init()
{
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    _rc = ServiceLocator::instance()->Get<RenderContext>();
    _hc = ServiceLocator::instance()->Get<HardwareContext>();
   
    return SyResult();
}

SyResult ShadowRenderSystem::Run()
{

    
    auto [camera, cameraTf] = CameraHelper::Find(_ecs, _ec->playModeState);

    auto viewLights = _ecs->view<LightComponent, TransformComponent>();
    for (auto& lightEnt : viewLights)
    {
        auto [light, lightTf] = viewLights.get(lightEnt);
        if (light.LightType == ELightType::Directional)
        {
            _hc->context->ClearDepthStencilView(light.DirShadowStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
            _hc->context->RSSetState(_rc->CullNoneRasterizerState.Get());
            _hc->context->OMSetRenderTargets(1, light.DirShadowRtv.GetAddressOf(), light.DirShadowStencilView.Get());
            //_rc->MRenderTargetTexture = nullptr;
            auto viewMeshes = _ecs->view<TransformComponent, MeshComponent>();
            for (auto& ent : viewMeshes)
            {
                auto [transform, meshComp] = viewMeshes.get(ent);

                if (!(meshComp.flags & SyEMeshComponentFlags::MESH_RENDER))
                    continue;
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
                HRESULT res = _hc->context->Map(_rc->ShadowConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                CopyMemory(mappedResource.pData, &dataShadow, sizeof(CB_ShadowBuffer));
                _hc->context->Unmap(_rc->ShadowConstBuffer->buffer.Get(), 0);
                _hc->context->VSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());
                _hc->context->PSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());
                _hc->context->GSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());

                for (auto& mesh : meshComp.model->meshes)
                {
                    UINT offset[1] = { 0 };
                    UINT stride[1] = { 48 };

                    D3D11_VIEWPORT viewport = {};
                    viewport.Width = static_cast<float>(_rc->ShadowmapWidth);
                    viewport.Height = static_cast<float>(_rc->ShadowmapHeight);
                    viewport.TopLeftX = 0;
                    viewport.TopLeftY = 0;
                    viewport.MinDepth = 0;
                    viewport.MaxDepth = 1.0f;

                    _hc->context->RSSetViewports(1, &viewport);

                    _hc->context->IASetInputLayout(_rc->ShadowShader->layout.Get());
                    _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    _hc->context->OMSetRenderTargets(1, light.DirShadowRtv.GetAddressOf(), light.DirShadowStencilView.Get());
                    _hc->context->VSSetShader(_rc->ShadowShader->vertexShader.Get(), nullptr, 0);
                    _hc->context->GSSetShader(_rc->ShadowShader->geomShader.Get(), nullptr, 0);
                    _hc->context->PSSetShader(_rc->ShadowShader->pixelShader.Get(), nullptr, 0);
                    _hc->context->IASetIndexBuffer(mesh->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                    //!_hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                    //    engineActor->model->strides, engineActor->model->offsets);
                    _hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                        meshComp.strides, meshComp.offsets);
                    _hc->context->DrawIndexedInstanced(mesh->indexBuffer->size, 4, 0, 0,0);
                }
            }
            BlurShadowMap(light);
        }
        else if (light.LightType == ELightType::PointLight && (light.LightBehavior==LightBehavior::Movable ||
            (light.LightBehavior==LightBehavior::Static && light.ShouldBakeShadows)))
        {
            pointlightShadowProjectionMat = DirectX::XMMatrixPerspectiveFovLH(1.5708f, _rc->ShadowmapWidth * 1.0f / _rc->ShadowmapHeight, 0.01f, light.ParamsRadiusAndAttenuation.x);

            light.ShouldBakeShadows = false;
            _hc->context->RSSetState(_rc->CullNoneRasterizerState.Get());
            _hc->context->ClearDepthStencilView(light.ShadowCubeMapDsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
            _hc->context->ClearRenderTargetView(light.ShadowCubeMapRtv.Get(),_rc->RhData.bgColor0001);
            

            CB_PointlightShadowBuffer dataShadow;
            dataShadow.world =Matrix::CreateScale(light.ParamsRadiusAndAttenuation.x, light.ParamsRadiusAndAttenuation.x,
                light.ParamsRadiusAndAttenuation.x)*Matrix::CreateTranslation(lightTf._position);
            
            //dataShadow.baseData.world = engineActor->transform->world * engineActor->transform->GetViewMatrix();
            dataShadow.view[0] = DirectX::XMMatrixLookAtLH(SyVector3(lightTf._position), lightTf._position + SyVector3(1, 0, 0),
                Vector3(0, 1, 0));
            dataShadow.view[1] = DirectX::XMMatrixLookAtLH(SyVector3(lightTf._position), lightTf._position + SyVector3(-1, 0, 0),
                Vector3(0, 1, 0));
            dataShadow.view[2] = DirectX::XMMatrixLookAtLH(SyVector3(lightTf._position), lightTf._position + SyVector3(0, 1, 0),
                Vector3(0, 0, -1));
            dataShadow.view[3] = DirectX::XMMatrixLookAtLH(SyVector3(lightTf._position), lightTf._position + SyVector3(0, -1, 0),
                Vector3(0, 0, 1));
            dataShadow.view[4] = DirectX::XMMatrixLookAtLH(SyVector3(lightTf._position), lightTf._position + SyVector3(0, 0, 1),
                Vector3(0, 1, 0));
            dataShadow.view[5] = DirectX::XMMatrixLookAtLH(SyVector3(lightTf._position), lightTf._position + SyVector3(0, 0, -1),
                Vector3(0, 1, 0));


            dataShadow.proj = pointlightShadowProjectionMat;
            
            auto viewMeshes = _ecs->view<TransformComponent, MeshComponent>();
            for (auto& ent : viewMeshes)
            {
                auto [transform, meshComp] = viewMeshes.get(ent);
                if (!(meshComp.flags & SyEMeshComponentFlags::MESH_RENDER))
                    continue;
                dataShadow.world = transform.transformMatrix;
                
                D3D11_MAPPED_SUBRESOURCE mappedResource;
                HRESULT res = _hc->context->Map(_rc->ShadowPointlightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                CopyMemory(mappedResource.pData, &dataShadow, sizeof(CB_PointlightShadowBuffer));
                _hc->context->Unmap(_rc->ShadowPointlightConstBuffer->buffer.Get(), 0);

                _hc->context->VSSetConstantBuffers(0, 1, _rc->ShadowPointlightConstBuffer->buffer.GetAddressOf());
                _hc->context->PSSetConstantBuffers(0, 1, _rc->ShadowPointlightConstBuffer->buffer.GetAddressOf());
                _hc->context->GSSetConstantBuffers(0, 1, _rc->ShadowPointlightConstBuffer->buffer.GetAddressOf());


                D3D11_VIEWPORT viewport = {};
                viewport.Width = static_cast<float>(_rc->ShadowmapWidth);
                viewport.Height = static_cast<float>(_rc->ShadowmapHeight);
                viewport.TopLeftX = 0;
                viewport.TopLeftY = 0;
                viewport.MinDepth = 0;
                viewport.MaxDepth = 1.0f;

                _hc->context->RSSetViewports(1, &viewport);

                _hc->context->IASetInputLayout(_rc->ShadowPointLightShader->layout.Get());
                _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                _hc->context->OMSetRenderTargets(1, light.ShadowCubeMapRtv.GetAddressOf(), light.ShadowCubeMapDsv.Get());
                _hc->context->PSSetShader(_rc->ShadowPointLightShader->pixelShader.Get(), nullptr, 0);
                _hc->context->VSSetShader(_rc->ShadowPointLightShader->vertexShader.Get(), nullptr, 0);
                _hc->context->GSSetShader(_rc->ShadowPointLightShader->geomShader.Get(), nullptr, 0);

                for (auto& mesh : meshComp.model->meshes)
                {
                    _hc->context->IASetIndexBuffer(mesh->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                    //!_hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                    //    engineActor->model->strides, engineActor->model->offsets);
                    _hc->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                        meshComp.strides, meshComp.offsets);
                    _hc->context->DrawIndexedInstanced(mesh->indexBuffer->size, 6, 0, 0,0);
                    
                }
               
            }
            _hc->context->PSSetShader(nullptr, nullptr, 0);
            _hc->context->GSSetShader(nullptr, nullptr, 0);
        }
    }

    _hc->context->GSSetShader(nullptr, nullptr, 0);

    _rc->Viewport.Width = static_cast<float>(_hc->window->GetWidth());
    _rc->Viewport.Height = static_cast<float>(_hc->window->GetHeight());
    _rc->Viewport.TopLeftX = 0;
    _rc->Viewport.TopLeftY = 0;
    _rc->Viewport.MinDepth = 0;
    _rc->Viewport.MaxDepth = 1.0f;
    _hc->context->RSSetViewports(1, &_rc->Viewport);

    return SyResult();
}

SyResult ShadowRenderSystem::Destroy()
{
    return SyResult();
}

void ShadowRenderSystem::BlurShadowMap(LightComponent& lc)
{

    _hc->context->OMSetDepthStencilState(_rc->OffStencilState.Get(), 0);
    _hc->context->VSSetShader(_rc->GaussianBlurX->vertexShader.Get(), nullptr, 0);
    _hc->context->PSSetShader(_rc->GaussianBlurX->pixelShader.Get(), nullptr, 0);
    _hc->context->GSSetShader(_rc->GaussianBlurX->geomShader.Get(), nullptr, 0);
    _hc->context->PSSetSamplers(0, 1, _rc->PointSampler.GetAddressOf());
    _hc->context->IASetInputLayout(_rc->GaussianBlurX->layout.Get());
    _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _hc->context->OMSetRenderTargets(1,lc.DirShadowBluredRtv.GetAddressOf() , nullptr);

    _hc->context->PSSetShaderResources(0, 1, lc.DirShadowSrv.GetAddressOf());
    _hc->context->IASetIndexBuffer(_rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    _hc->context->IASetVertexBuffers(0, 1, _rc->VertexQuadBuffer->buffer.GetAddressOf(),
        _rc->RhData.strides32, _rc->RhData.offsets0);

    _hc->context->DrawIndexedInstanced(6, 4, 0, 0, 0);

    _hc->context->CopyResource(lc.DirShadowRtTexture.Get(), lc.DirBluredShadowTexture.Get());

    _hc->context->PSSetShader(_rc->GaussianBlurY->pixelShader.Get(), nullptr, 0);
    _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _hc->context->OMSetRenderTargets(1, lc.DirShadowBluredRtv.GetAddressOf(), nullptr);
    _hc->context->DrawIndexedInstanced(6, 4, 0, 0, 0);

    _hc->context->OMSetDepthStencilState(_hc->depthStencilState.Get(), 0);
}
