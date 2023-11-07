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
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    _hc = ServiceLocator::instance()->Get<HardwareContext>();
    _rc = ServiceLocator::instance()->Get<RenderContext>();
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "ShadowMapGeneration system initialization successful. ");
    return SyResult();
}

SyResult ShadowMapGenerationSystem::Run()
{
    CB_LightBuffer lightBuffer;



    ID3D11ShaderResourceView* resources[] = { _rc->GBuffer->DiffuseSrv.Get(),_rc->GBuffer->NormalAndDepthSrv.Get(),_rc->GBuffer->PositionSrv.Get(),_rc->GBuffer->IdSrv.Get(),_rc->GBuffer->DepthSrv.Get() };


    _hc->context->ClearRenderTargetView(_rc->ShadowMapRTV, _rc->RhData.bgColor0001);



    _hc->context->OMSetBlendState(_rc->LightBlendState.Get(), nullptr, 0xffffffff);

    auto [camera, cameraTf] = CameraHelper::Find(_ecs);

    auto view = _ecs->view<TransformComponent, LightComponent>();
    for (auto& entity : view)
    {
        LightComponent& light = view.get<LightComponent>(entity);
        TransformComponent& tc = view.get<TransformComponent>(entity);
        if (light.LightType == ELightType::Directional)
        {
            lightBuffer.lightData.Pos = Vector4(tc._position.x, tc._position.y, tc._position.z, 1);
            lightBuffer.lightData.Color = light.Color;
            lightBuffer.lightData.Dir = Vector4::Transform(Vector4::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation));
            lightBuffer.lightData.additiveParams = light.ParamsRadiusAndAttenuation;
            lightBuffer.eyePos = Vector4(
                cameraTf._position.x,
                cameraTf._position.y,
                cameraTf._position.z,
                1.0f);

            _hc->context->ClearDepthStencilView(_hc->depthStencilView.Get(), D3D11_CLEAR_STENCIL, 1, 0);

            if (light.LightType == ELightType::Directional || light.LightType == ELightType::Ambient)
            {
                lightBuffer.baseData.world = Matrix::Identity;
                lightBuffer.baseData.worldView = Matrix::Identity * camera.view;
                lightBuffer.baseData.worldViewProj = Matrix::Identity * camera.view * camera.projection;
                lightBuffer.baseData.worldViewInverseTranspose = Matrix::Identity;

                if (light.LightType == ELightType::Directional)
                    for (int i = 0; i < 4; i++)
                    {
                        lightBuffer.distances[i] = light.Distances[i];
                        lightBuffer.viewProjs[i] = light.ViewMatrices[i] * light.OrthoMatrices[i];
                    }
            }
            else if (light.LightType == ELightType::PointLight)
            {
                lightBuffer.lightData.Pos = Vector4(tc._position.x, tc._position.y, tc._position.z, 1);
                lightBuffer.lightData.additiveParams = light.ParamsRadiusAndAttenuation;
                lightBuffer.baseData.world = Matrix::CreateScale(light.ParamsRadiusAndAttenuation.x,
                    light.ParamsRadiusAndAttenuation.x,light.ParamsRadiusAndAttenuation.x)*tc.transformMatrix;

            }


            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT res = _hc->context->Map(_rc->LightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            CopyMemory(mappedResource.pData, &lightBuffer, sizeof(CB_LightBuffer));
            _hc->context->Unmap(_rc->LightConstBuffer->buffer.Get(), 0);
            _hc->context->VSSetConstantBuffers(0, 1, _rc->LightConstBuffer->buffer.GetAddressOf());
            _hc->context->PSSetConstantBuffers(0, 1, _rc->LightConstBuffer->buffer.GetAddressOf());


            //_hc->context->OMSetBlendState(_rc->LightBlendState.Get(), nullptr, 0xffffffff);
            _hc->context->PSSetSamplers(0, 1, _rc->SamplerState.GetAddressOf());
            _hc->context->PSSetSamplers(1, 1, _rc->SamplerDepthState.GetAddressOf());
            _hc->context->OMSetRenderTargets(1, &_rc->ShadowMapRTV, nullptr);
            

            if (light.LightType == ELightType::Directional)
            {
                _hc->context->PSSetShaderResources(0, 5, resources);
                _hc->context->RSSetState(_rc->CullBackRasterizerState.Get());
                _hc->context->OMSetDepthStencilState(_rc->OffStencilState.Get(), 0);
                _hc->context->VSSetShader(_rc->ShadowMapGenerator->vertexShader.Get(), nullptr, 0);
                _hc->context->PSSetShader(_rc->ShadowMapGenerator->pixelShader.Get(), nullptr, 0);
                _hc->context->PSSetShaderResources(5, 1, light.ShadowResourceView.GetAddressOf());
                //_hc->context->PSSetShaderResources(6, 1, &_rc->SkyboxSRV);
                _hc->context->IASetInputLayout(_rc->ShadowMapGenerator->layout.Get());
                _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
                _hc->context->IASetIndexBuffer(_rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                _hc->context->IASetVertexBuffers(0, 1, _rc->VertexQuadBuffer->buffer.GetAddressOf(),
                    _rc->RhData.strides32, _rc->RhData.offsets0);
                _hc->context->DrawIndexed(6, 0, 0);
            }
            else if (light.LightType == ELightType::PointLight)
            {
                _hc->context->RSSetState(_rc->CullBackRasterizerState.Get());
                _hc->context->OMSetDepthStencilState(_rc->OffStencilState.Get(), 0);
                _hc->context->VSSetShader(_rc->ShadowMapPointLightGenerator->vertexShader.Get(), nullptr, 0);
                _hc->context->PSSetShader(_rc->ShadowMapPointLightGenerator->pixelShader.Get(), nullptr, 0);
                _hc->context->PSSetShaderResources(0, 1, light.ShadowCubeMapSrv.GetAddressOf());
                _hc->context->PSSetShaderResources(1, 1, _rc->GBuffer->PositionSrv.GetAddressOf());
                //_hc->context->PSSetShaderResources(6, 1, &_rc->SkyboxSRV);
                _hc->context->IASetInputLayout(_rc->ShadowMapPointLightGenerator->layout.Get());
                _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
                _hc->context->IASetIndexBuffer(_rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                _hc->context->IASetVertexBuffers(0, 1, _rc->VertexQuadBuffer->buffer.GetAddressOf(),
                    _rc->RhData.strides32, _rc->RhData.offsets0);
                _hc->context->DrawIndexed(6, 0, 0);
            }
        }
    }
    _hc->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    _hc->context->PSSetShaderResources(0, 5, _rc->RhData.NullSrv);
    return SyResult();
}

SyResult ShadowMapGenerationSystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "ShadowMapGeneration system destruction successful. ");
    return SyResult();
}
