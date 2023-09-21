#include "ShadowMapGenerationSystem.h"
#include "../RenderContext.h"
#include "../../HardwareContext.h"
#include "../../EngineContext.h"
#include "../../../Core/Graphics/ConstantBuffer.h"
#include "../../../Core/ServiceLocator.h"

void ShadowMapGenerationSystem::Init()
{
    ec = ServiceLocator::instance()->Get<EngineContext>();
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
}

void ShadowMapGenerationSystem::Run()
{
    CB_LightBuffer lightBuffer;

    float bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    ID3D11ShaderResourceView* srvNull[] = { nullptr,nullptr,nullptr,nullptr,nullptr };

    ID3D11ShaderResourceView* resources[] = { rc->gBuffer->diffuseSRV.Get(),rc->gBuffer->normalSRV.Get(),rc->gBuffer->positionSRV.Get(),rc->gBuffer->depthSRV.Get(),rc->gBuffer->specularSRV.Get() };
    //hc->context->ClearRenderTargetView(engine->rtv.Get(), bgColor);
    //hc->renderTarget->ClearRenderTarget(hc->depthStencilView.Get(), D3D11_CLEAR_STENCIL);

    hc->context->ClearRenderTargetView(rc->shadowMapRTV, bgColor);
    UINT strides[1] = { 32 };
    UINT offsets[1] = { 0 };

    UINT stridesLight[1] = { 48 };
    UINT offsetsLight[1] = { 0 };

    auto view = ec->scene->registry.view<TransformComponent, LightComponent>();
    hc->context->OMSetBlendState(rc->lightBlendState.Get(), nullptr, 0xffffffff);
    for (auto& entity : view)
    {
        
        LightComponent& light = view.get<LightComponent>(entity);
        TransformComponent& tc = view.get<TransformComponent>(entity);
        if (light.lightType == LightType::Directional || light.lightType == LightType::PointLight)
        {
            lightBuffer.lightData.Pos = Vector4(tc.localPosition.x, tc.localPosition.y, tc.localPosition.z, 1);
            lightBuffer.lightData.Color = light.color;
            lightBuffer.lightData.Dir = Vector4::Transform(Vector4::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation));
            lightBuffer.lightData.additiveParams = light.paramsRadiusAndAttenuation;
            lightBuffer.eyePos = DirectX::SimpleMath::Vector4(ec->scene->cameraTransform->localPosition.x,
                ec->scene->cameraTransform->localPosition.y,
                ec->scene->cameraTransform->localPosition.z,
                1.0f);

            hc->context->ClearDepthStencilView(hc->depthStencilView.Get(), D3D11_CLEAR_STENCIL, 1, 0);

            if (light.lightType == LightType::Directional || light.lightType == LightType::Ambient)
            {
                lightBuffer.baseData.world = DirectX::SimpleMath::Matrix::Identity;
                lightBuffer.baseData.worldView = DirectX::SimpleMath::Matrix::Identity * ec->scene->camera->view;
                lightBuffer.baseData.worldViewProj = DirectX::SimpleMath::Matrix::Identity * ec->scene->camera->view * ec->scene->camera->projection;
                lightBuffer.baseData.worldViewInverseTranspose = DirectX::SimpleMath::Matrix::Identity;

                if (light.lightType == LightType::Directional)
                    for (int i = 0; i < 4; i++)
                    {
                        lightBuffer.distances[i] = light.distances[i];
                        lightBuffer.viewProjs[i] = light.viewMatrices[i] * light.orthoMatrices[i];
                    }
            }
            else if (light.lightType == LightType::PointLight)
            {
                lightBuffer.lightData.Pos = Vector4(tc.localPosition.x, tc.localPosition.y, tc.localPosition.z, 25);
                lightBuffer.lightData.additiveParams = light.paramsRadiusAndAttenuation;
                lightBuffer.baseData.world = tc.transformMatrix;

            }


            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT res = hc->context->Map(rc->lightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            CopyMemory(mappedResource.pData, &lightBuffer, sizeof(CB_LightBuffer));
            hc->context->Unmap(rc->lightConstBuffer->buffer.Get(), 0);
            hc->context->VSSetConstantBuffers(0, 1, rc->lightConstBuffer->buffer.GetAddressOf());
            hc->context->PSSetConstantBuffers(0, 1, rc->lightConstBuffer->buffer.GetAddressOf());


            //hc->context->OMSetBlendState(rc->lightBlendState.Get(), nullptr, 0xffffffff);
            hc->context->PSSetSamplers(0, 1, rc->samplerState.GetAddressOf());
            hc->context->PSSetSamplers(1, 1, rc->samplerDepthState.GetAddressOf());
            hc->context->OMSetRenderTargets(1, &rc->shadowMapRTV, nullptr);
            

            if (light.lightType == LightType::Directional)
            {
                hc->context->PSSetShaderResources(0, 5, resources);
                hc->context->RSSetState(rc->cullBackRS.Get());
                hc->context->OMSetDepthStencilState(rc->offStencilState.Get(), 0);
                hc->context->VSSetShader(rc->shadowMapGenerator->vertexShader.Get(), nullptr, 0);
                hc->context->PSSetShader(rc->shadowMapGenerator->pixelShader.Get(), nullptr, 0);
                hc->context->PSSetShaderResources(5, 1, &rc->shadowResourceView);
                //hc->context->PSSetShaderResources(6, 1, &rc->skyboxSRV);
                hc->context->IASetInputLayout(rc->shadowMapGenerator->layout.Get());
                hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
                hc->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                hc->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
                    strides, offsets);
                hc->context->DrawIndexed(6, 0, 0);
            }
            else if (light.lightType == LightType::PointLight)
            {
                hc->context->RSSetState(rc->cullBackRS.Get());
                hc->context->OMSetDepthStencilState(rc->offStencilState.Get(), 0);
                hc->context->VSSetShader(rc->shadowMapPointLightGenerator->vertexShader.Get(), nullptr, 0);
                hc->context->PSSetShader(rc->shadowMapPointLightGenerator->pixelShader.Get(), nullptr, 0);
                hc->context->PSSetShaderResources(0, 1, light.shadowMapSRV.GetAddressOf());
                hc->context->PSSetShaderResources(1, 1, rc->gBuffer->positionSRV.GetAddressOf());
                //hc->context->PSSetShaderResources(6, 1, &rc->skyboxSRV);
                hc->context->IASetInputLayout(rc->shadowMapPointLightGenerator->layout.Get());
                hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
                hc->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                hc->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
                    strides, offsets);
                hc->context->DrawIndexed(6, 0, 0);
            }
        }
    }
    hc->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    hc->context->PSSetShaderResources(0, 5, srvNull);
}

void ShadowMapGenerationSystem::Destroy()
{
}
