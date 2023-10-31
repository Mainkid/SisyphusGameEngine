#include "ShadowMapGenerationSystem.h"
#include "../RenderContext.h"
#include "../../HardwareContext.h"
#include "../../EngineContext.h"
#include "../../../Components/CameraComponent.h"
#include "../../../Core/Graphics/ConstantBuffer.h"
#include "../../../Core/ServiceLocator.h"
#include "../../../Scene/CameraHelper.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/LightComponent.h"

SyResult ShadowMapGenerationSystem::Init()
{
    ec = ServiceLocator::instance()->Get<EngineContext>();
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "ShadowMapGeneration system initialization successful. ");
    return SyResult();
}

SyResult ShadowMapGenerationSystem::Run()
{
    CB_LightBuffer lightBuffer;

    float bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    ID3D11ShaderResourceView* srvNull[] = { nullptr,nullptr,nullptr,nullptr,nullptr };

    ID3D11ShaderResourceView* resources[] = { rc->GBuffer->DiffuseSrv.Get(),rc->GBuffer->NormalSrv.Get(),rc->GBuffer->PositionSrv.Get(),rc->GBuffer->IdSrv.Get(),rc->GBuffer->HbaoSrv.Get() };
    //hc->context->ClearRenderTargetView(engine->rtv.Get(), bgColor);
    //hc->renderTarget->ClearRenderTarget(hc->depthStencilView.Get(), D3D11_CLEAR_STENCIL);

    hc->context->ClearRenderTargetView(rc->ShadowMapRTV, bgColor);
    UINT strides[1] = { 32 };
    UINT offsets[1] = { 0 };

    UINT stridesLight[1] = { 48 };
    UINT offsetsLight[1] = { 0 };

    hc->context->OMSetBlendState(rc->LightBlendState.Get(), nullptr, 0xffffffff);

    auto [camera, cameraTf] = CameraHelper::Find(_ecs);

    auto view = _ecs->view<TransformComponent, LightComponent>();
    for (auto& entity : view)
    {
        LightComponent& light = view.get<LightComponent>(entity);
        TransformComponent& tc = view.get<TransformComponent>(entity);
        if (light.LightType == LightType::Directional)
        {
            lightBuffer.lightData.Pos = Vector4(tc.position.x, tc.position.y, tc.position.z, 1);
            lightBuffer.lightData.Color = light.Color;
            lightBuffer.lightData.Dir = Vector4::Transform(Vector4::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation));
            lightBuffer.lightData.additiveParams = light.ParamsRadiusAndAttenuation;
            lightBuffer.eyePos = Vector4(
                cameraTf.position.x,
                cameraTf.position.y,
                cameraTf.position.z,
                1.0f);

            hc->context->ClearDepthStencilView(hc->depthStencilView.Get(), D3D11_CLEAR_STENCIL, 1, 0);

            if (light.LightType == LightType::Directional || light.LightType == LightType::Ambient)
            {
                lightBuffer.baseData.world = Matrix::Identity;
                lightBuffer.baseData.worldView = Matrix::Identity * camera.view;
                lightBuffer.baseData.worldViewProj = Matrix::Identity * camera.view * camera.projection;
                lightBuffer.baseData.worldViewInverseTranspose = Matrix::Identity;

                if (light.LightType == LightType::Directional)
                    for (int i = 0; i < 4; i++)
                    {
                        lightBuffer.distances[i] = light.Distances[i];
                        lightBuffer.viewProjs[i] = light.ViewMatrices[i] * light.OrthoMatrices[i];
                    }
            }
            else if (light.LightType == LightType::PointLight)
            {
                lightBuffer.lightData.Pos = Vector4(tc.position.x, tc.position.y, tc.position.z, 1);
                lightBuffer.lightData.additiveParams = light.ParamsRadiusAndAttenuation;
                lightBuffer.baseData.world = Matrix::CreateScale(light.ParamsRadiusAndAttenuation.x,
                    light.ParamsRadiusAndAttenuation.x,light.ParamsRadiusAndAttenuation.x)*tc.transformMatrix;

            }


            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT res = hc->context->Map(rc->LightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            CopyMemory(mappedResource.pData, &lightBuffer, sizeof(CB_LightBuffer));
            hc->context->Unmap(rc->LightConstBuffer->buffer.Get(), 0);
            hc->context->VSSetConstantBuffers(0, 1, rc->LightConstBuffer->buffer.GetAddressOf());
            hc->context->PSSetConstantBuffers(0, 1, rc->LightConstBuffer->buffer.GetAddressOf());


            //hc->context->OMSetBlendState(rc->LightBlendState.Get(), nullptr, 0xffffffff);
            hc->context->PSSetSamplers(0, 1, rc->SamplerState.GetAddressOf());
            hc->context->PSSetSamplers(1, 1, rc->SamplerDepthState.GetAddressOf());
            hc->context->OMSetRenderTargets(1, &rc->ShadowMapRTV, nullptr);
            

            if (light.LightType == LightType::Directional)
            {
                hc->context->PSSetShaderResources(0, 5, resources);
                hc->context->RSSetState(rc->CullBackRasterizerState.Get());
                hc->context->OMSetDepthStencilState(rc->OffStencilState.Get(), 0);
                hc->context->VSSetShader(rc->ShadowMapGenerator->vertexShader.Get(), nullptr, 0);
                hc->context->PSSetShader(rc->ShadowMapGenerator->pixelShader.Get(), nullptr, 0);
                hc->context->PSSetShaderResources(5, 1, &rc->ShadowResourceView);
                //hc->context->PSSetShaderResources(6, 1, &rc->SkyboxSRV);
                hc->context->IASetInputLayout(rc->ShadowMapGenerator->layout.Get());
                hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
                hc->context->IASetIndexBuffer(rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                hc->context->IASetVertexBuffers(0, 1, rc->VertexQuadBuffer->buffer.GetAddressOf(),
                    strides, offsets);
                hc->context->DrawIndexed(6, 0, 0);
            }
            else if (light.LightType == LightType::PointLight)
            {
                hc->context->RSSetState(rc->CullBackRasterizerState.Get());
                hc->context->OMSetDepthStencilState(rc->OffStencilState.Get(), 0);
                hc->context->VSSetShader(rc->ShadowMapPointLightGenerator->vertexShader.Get(), nullptr, 0);
                hc->context->PSSetShader(rc->ShadowMapPointLightGenerator->pixelShader.Get(), nullptr, 0);
                hc->context->PSSetShaderResources(0, 1, light.ShadowCubeMapSrv.GetAddressOf());
                hc->context->PSSetShaderResources(1, 1, rc->GBuffer->PositionSrv.GetAddressOf());
                //hc->context->PSSetShaderResources(6, 1, &rc->SkyboxSRV);
                hc->context->IASetInputLayout(rc->ShadowMapPointLightGenerator->layout.Get());
                hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
                hc->context->IASetIndexBuffer(rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                hc->context->IASetVertexBuffers(0, 1, rc->VertexQuadBuffer->buffer.GetAddressOf(),
                    strides, offsets);
                hc->context->DrawIndexed(6, 0, 0);
            }
        }
    }
    hc->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    hc->context->PSSetShaderResources(0, 5, srvNull);
    return SyResult();
}

SyResult ShadowMapGenerationSystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "ShadowMapGeneration system destruction successful. ");
    return SyResult();
}
