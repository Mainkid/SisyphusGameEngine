#include "LightRenderSystem.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../RenderContext.h"
#include "../../Core/Graphics/ConstantBuffer.h"

SyResult LightRenderSystem::Init()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    ec = ServiceLocator::instance()->Get<EngineContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "LightRender system initialization successful.");
    return SyResult();

}

SyResult LightRenderSystem::Run()
{

    CB_LightBuffer lightBuffer;

    float bgColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    ID3D11ShaderResourceView* srvNull[] = { nullptr,nullptr,nullptr,nullptr,nullptr };

    ID3D11ShaderResourceView* resources[] = { rc->gBuffer->diffuseSRV.Get(),rc->gBuffer->normalSRV.Get(),rc->gBuffer->positionSRV.Get(),rc->gBuffer->depthSRV.Get(),rc->gBuffer->specularSRV.Get() };
    //hc->context->ClearRenderTargetView(engine->rtv.Get(), bgColor);
    hc->renderTarget->ClearRenderTarget(hc->depthStencilView.Get(), D3D11_CLEAR_STENCIL);
    UINT strides[1] = { 32 };
    UINT offsets[1] = { 0 };

    UINT stridesLight[1] = { 48 };
    UINT offsetsLight[1] = { 0 };

    auto view = ec->scene->registry.view<TransformComponent, LightComponent>();
    for (auto& entity : view)
    {

        LightComponent& light = view.get<LightComponent>(entity);
        TransformComponent& tc = view.get<TransformComponent>(entity);
        lightBuffer.lightData.Pos = Vector4(tc.position.x, tc.position.y, tc.position.z, 1);
        lightBuffer.lightData.Color = light.color;
        lightBuffer.lightData.Dir = Vector4::Transform(Vector4::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation));
        lightBuffer.lightData.additiveParams = light.paramsRadiusAndAttenuation;
        lightBuffer.eyePos = DirectX::SimpleMath::Vector4(ec->scene->cameraTransform->position.x,
            ec->scene->cameraTransform->position.y,
            ec->scene->cameraTransform->position.z,
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
            using namespace DirectX::SimpleMath;
            lightBuffer.baseData.world = Matrix::CreateScale(light.paramsRadiusAndAttenuation.x, light.paramsRadiusAndAttenuation.x, light.paramsRadiusAndAttenuation.x) * Matrix::CreateTranslation(tc.position);

            lightBuffer.baseData.worldView = lightBuffer.baseData.world * ec->scene->camera->view;
            lightBuffer.baseData.worldViewProj = lightBuffer.baseData.worldView * ec->scene->camera->projection;
            lightBuffer.baseData.worldViewInverseTranspose = DirectX::SimpleMath::Matrix::Identity;
        }
        else if (light.lightType == LightType::SpotLight)
        {
            using namespace DirectX::SimpleMath;
            Vector3 a;
            Vector3 forward = Vector3(0, 0, 1);
            Vector3 dir = Vector3::Transform(Vector3::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation));
            dir.Normalize();
            forward.Cross(dir, a);
            a.Normalize();
            float angle = acos(forward.Dot(dir));
            float radius = sqrt(pow((light.paramsRadiusAndAttenuation.x / light.paramsRadiusAndAttenuation.y), 2.0f) - pow(light.paramsRadiusAndAttenuation.x, 2.0f));

            lightBuffer.baseData.world = Matrix::CreateTranslation(Vector3(0,
                0,
                1.0f)) *
                Matrix::CreateScale(radius, radius, light.paramsRadiusAndAttenuation.x) *
                Matrix::CreateFromAxisAngle(a, angle) *
                Matrix::CreateTranslation(tc.position);
            lightBuffer.baseData.worldView = lightBuffer.baseData.world * ec->scene->camera->view;
            lightBuffer.baseData.worldViewProj = lightBuffer.baseData.worldView * ec->scene->camera->projection;
            lightBuffer.baseData.worldViewInverseTranspose = DirectX::SimpleMath::Matrix::Identity;


        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT res = hc->context->Map(rc->lightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &lightBuffer, sizeof(CB_LightBuffer));
        hc->context->Unmap(rc->lightConstBuffer->buffer.Get(), 0);
        hc->context->VSSetConstantBuffers(0, 1, rc->lightConstBuffer->buffer.GetAddressOf());
        hc->context->PSSetConstantBuffers(0, 1, rc->lightConstBuffer->buffer.GetAddressOf());


        hc->context->OMSetBlendState(rc->lightBlendState.Get(), nullptr, 0xffffffff);
        hc->context->PSSetSamplers(0, 1, rc->samplerState.GetAddressOf());
        hc->context->PSSetSamplers(1, 1, rc->samplerDepthState.GetAddressOf());

        //hc->context->OMSetRenderTargets(1, engine->rtv.GetAddressOf(), engine->depthStencilView.Get());
        hc->renderTarget->SetRenderTarget(hc->depthStencilView.Get());
        hc->context->PSSetShaderResources(0, 5, resources);

        if (light.lightType == LightType::Ambient || light.lightType == LightType::Directional)
        {
            hc->context->RSSetState(rc->cullBackRS.Get());
            hc->context->OMSetDepthStencilState(rc->offStencilState.Get(), 0);
            hc->context->VSSetShader(rc->dirLightShader->vertexShader.Get(), nullptr, 0);
            if (light.lightType == LightType::Directional)
            {
                hc->context->PSSetShader(rc->dirLightShader->pixelShader.Get(), nullptr, 0);
                hc->context->PSSetShaderResources(5, 1, &rc->shadowMapResourceView);
            }
            else
            {
                hc->context->PSSetShader(rc->ambientLightShader->pixelShader.Get(), nullptr, 0);
                hc->context->PSSetShaderResources(6, 1, rc->gBuffer->skyboxSRV.GetAddressOf());
                //hc->context->PSSetShaderResources(7, 1, hc->depthStencilView.GetAddressOf());
            }

            hc->context->IASetInputLayout(rc->dirLightShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
                strides, offsets);
            hc->context->DrawIndexed(6, 0, 0);
        }
        else if (light.lightType == LightType::PointLight)
        {

            //Back Face Pass
            hc->context->PSSetShaderResources(5, 1, light.shadowMapSRV.GetAddressOf());

            hc->context->RSSetState(rc->cullFrontRS.Get());
            hc->context->OMSetDepthStencilState(rc->backFaceStencilState.Get(), 0);
            hc->context->PSSetShader(rc->stencilPassShader->pixelShader.Get(), nullptr, 0);
            hc->context->VSSetShader(rc->stencilPassShader->vertexShader.Get(), nullptr, 0);
            hc->context->IASetInputLayout(rc->stencilPassShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(light.aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, light.aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            hc->context->DrawIndexed(light.aabb->indexBuffer->size, 0, 0);


            //Front Face Pass
            hc->context->RSSetState(rc->cullBackRS.Get());
            hc->context->OMSetDepthStencilState(rc->frontFaceStencilState.Get(), 1);
            hc->context->VSSetShader(rc->stencilPassShader->vertexShader.Get(), nullptr, 0);
            hc->context->PSSetShader(rc->stencilPassShader->pixelShader.Get(), nullptr, 0);
            hc->context->VSSetConstantBuffers(0, 1, rc->lightConstBuffer->buffer.GetAddressOf());
            hc->context->IASetInputLayout(rc->stencilPassShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(light.aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, light.aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            hc->context->DrawIndexed(light.aabb->indexBuffer->size, 0, 0);


            //Final Pass

            hc->context->RSSetState(rc->cullBackRS.Get());
            hc->context->OMSetDepthStencilState(rc->finalPassStencilState.Get(), 0);
            hc->context->VSSetShader(rc->dirLightShader->vertexShader.Get(), nullptr, 0);

            hc->context->PSSetShader(rc->pointLightShader->pixelShader.Get(), nullptr, 0);
            hc->context->IASetInputLayout(rc->dirLightShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
                strides, offsets);

            hc->context->DrawIndexed(6, 0, 0);


        }

        else if (light.lightType == LightType::SpotLight)
        {
            //Back Face Pass

            hc->context->RSSetState(rc->cullFrontRS.Get());
            hc->context->OMSetDepthStencilState(rc->backFaceStencilState.Get(), 0);
            hc->context->PSSetShader(rc->stencilPassShader->pixelShader.Get(), nullptr, 0);
            hc->context->VSSetShader(rc->stencilPassShader->vertexShader.Get(), nullptr, 0);
            hc->context->IASetInputLayout(rc->stencilPassShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(light.aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, light.aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            hc->context->DrawIndexed(light.aabb->indexBuffer->size, 0, 0);


            //Front Face Pass

            hc->context->RSSetState(rc->cullBackRS.Get());
            hc->context->OMSetDepthStencilState(rc->frontFaceStencilState.Get(), 1); //change???
            hc->context->VSSetShader(rc->stencilPassShader->vertexShader.Get(), nullptr, 0);
            hc->context->PSSetShader(rc->stencilPassShader->pixelShader.Get(), nullptr, 0);
            //hc->context->PSSetShader(spotLightShader->pixelShader.Get(),nullptr,0);
            hc->context->VSSetConstantBuffers(0, 1, rc->lightConstBuffer->buffer.GetAddressOf());
            hc->context->IASetInputLayout(rc->stencilPassShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(light.aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, light.aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            hc->context->DrawIndexed(light.aabb->indexBuffer->size, 0, 0);

            //Final Pass

            hc->context->RSSetState(rc->cullBackRS.Get());
            hc->context->OMSetDepthStencilState(rc->finalPassStencilState.Get(), 0);
            hc->context->VSSetShader(rc->dirLightShader->vertexShader.Get(), nullptr, 0);
            hc->context->PSSetShader(rc->spotLightShader->pixelShader.Get(), nullptr, 0);
            hc->context->IASetInputLayout(rc->dirLightShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
                strides, offsets);

            hc->context->DrawIndexed(6, 0, 0);

        }

    }
    hc->context->PSSetShaderResources(0, 5, srvNull);
    return SyResult();
}

SyResult LightRenderSystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "LightRender system destruction successful.");
    return SyResult();
}

void LightRenderSystem::ShadowMap()
{
    
}
