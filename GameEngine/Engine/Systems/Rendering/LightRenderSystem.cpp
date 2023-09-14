#include "LightRenderSystem.h"
#include "../../Core/EngineCore.h"
#include "../../Core/Graphics/ConstantBuffer.h"

void LightRenderSystem::Init()
{
    engine = EngineCore::instance();
    rc = ServiceLocator::instance()->Get<RenderContext>();
}

void LightRenderSystem::Run()
{
    CB_LightBuffer lightBuffer;

    float bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    ID3D11ShaderResourceView* srvNull[] = { nullptr,nullptr,nullptr,nullptr,nullptr };

    ID3D11ShaderResourceView* resources[] = { rc->gBuffer->diffuseSRV.Get(),rc->gBuffer->normalSRV.Get(),rc->gBuffer->positionSRV.Get(),rc->gBuffer->depthSRV.Get(),rc->gBuffer->specularSRV.Get() };
    //engine->context->ClearRenderTargetView(engine->rtv.Get(), bgColor);
    engine->renderTarget->ClearRenderTarget(engine->depthStencilView.Get(), D3D11_CLEAR_STENCIL);
    UINT strides[1] = { 32 };
    UINT offsets[1] = { 0 };

    UINT stridesLight[1] = { 48 };
    UINT offsetsLight[1] = { 0 };

    auto view = engine->scene->registry.view<TransformComponent, LightComponent>();
    for (auto& entity : view)
    {

        LightComponent& light = view.get<LightComponent>(entity);
        TransformComponent& tc = view.get<TransformComponent>(entity);
        lightBuffer.lightData.Pos = Vector4(tc.localPosition.x, tc.localPosition.y, tc.localPosition.z, 1);
        lightBuffer.lightData.Color = light.color;
        lightBuffer.lightData.Dir = Vector4::Transform(Vector4::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation));
        lightBuffer.lightData.additiveParams = light.paramsRadiusAndAttenuation;
        lightBuffer.eyePos = DirectX::SimpleMath::Vector4(engine->scene->cameraTransform->localPosition.x,
            engine->scene->cameraTransform->localPosition.y,
            engine->scene->cameraTransform->localPosition.z,
            1.0f);

        engine->context->ClearDepthStencilView(engine->depthStencilView.Get(), D3D11_CLEAR_STENCIL, 1, 0);

        if (light.lightType == LightType::Directional || light.lightType == LightType::Ambient)
        {
            lightBuffer.baseData.world = DirectX::SimpleMath::Matrix::Identity;
            lightBuffer.baseData.worldView = DirectX::SimpleMath::Matrix::Identity * engine->scene->camera->view;
            lightBuffer.baseData.worldViewProj = DirectX::SimpleMath::Matrix::Identity * engine->scene->camera->view * engine->scene->camera->projection;
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
            lightBuffer.baseData.world = Matrix::CreateScale(light.paramsRadiusAndAttenuation.x, light.paramsRadiusAndAttenuation.x, light.paramsRadiusAndAttenuation.x) * Matrix::CreateTranslation(tc.localPosition);

            lightBuffer.baseData.worldView = lightBuffer.baseData.world * engine->scene->camera->view;
            lightBuffer.baseData.worldViewProj = lightBuffer.baseData.worldView * engine->scene->camera->projection;
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
                Matrix::CreateTranslation(tc.localPosition);
            lightBuffer.baseData.worldView = lightBuffer.baseData.world * engine->scene->camera->view;
            lightBuffer.baseData.worldViewProj = lightBuffer.baseData.worldView * engine->scene->camera->projection;
            lightBuffer.baseData.worldViewInverseTranspose = DirectX::SimpleMath::Matrix::Identity;


        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT res = engine->context->Map(rc->lightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &lightBuffer, sizeof(CB_LightBuffer));
        engine->context->Unmap(rc->lightConstBuffer->buffer.Get(), 0);
        engine->context->VSSetConstantBuffers(0, 1, rc->lightConstBuffer->buffer.GetAddressOf());
        engine->context->PSSetConstantBuffers(0, 1, rc->lightConstBuffer->buffer.GetAddressOf());


        engine->context->OMSetBlendState(rc->lightBlendState.Get(), nullptr, 0xffffffff);
        engine->context->PSSetSamplers(0, 1, rc->samplerState.GetAddressOf());
        engine->context->PSSetSamplers(1, 1, rc->samplerDepthState.GetAddressOf());

        //engine->context->OMSetRenderTargets(1, engine->rtv.GetAddressOf(), engine->depthStencilView.Get());
        engine->renderTarget->SetRenderTarget(engine->depthStencilView.Get());
        engine->context->PSSetShaderResources(0, 5, resources);

        if (light.lightType == LightType::Ambient || light.lightType == LightType::Directional)
        {
            engine->context->RSSetState(rc->cullBackRS.Get());
            engine->context->OMSetDepthStencilState(rc->offStencilState.Get(), 0);
            engine->context->VSSetShader(rc->dirLightShader->vertexShader.Get(), nullptr, 0);
            if (light.lightType == LightType::Directional)
            {
                engine->context->PSSetShader(rc->dirLightShader->pixelShader.Get(), nullptr, 0);
                engine->context->PSSetShaderResources(5, 1, &rc->shadowResourceView);
            }
            else
                engine->context->PSSetShader(rc->ambientLightShader->pixelShader.Get(), nullptr, 0);

            engine->context->IASetInputLayout(rc->dirLightShader->layout.Get());
            engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            engine->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            engine->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
                strides, offsets);
            engine->context->DrawIndexed(6, 0, 0);
        }
        else if (light.lightType == LightType::PointLight)
        {

            //Back Face Pass


            engine->context->RSSetState(rc->cullFrontRS.Get());
            engine->context->OMSetDepthStencilState(rc->backFaceStencilState.Get(), 0);
            engine->context->PSSetShader(rc->stencilPassShader->pixelShader.Get(), nullptr, 0);
            engine->context->VSSetShader(rc->stencilPassShader->vertexShader.Get(), nullptr, 0);
            engine->context->IASetInputLayout(rc->stencilPassShader->layout.Get());
            engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            engine->context->IASetIndexBuffer(light.aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            engine->context->IASetVertexBuffers(0, 1, light.aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            engine->context->DrawIndexed(light.aabb->indexBuffer->size, 0, 0);


            //Front Face Pass
            engine->context->RSSetState(rc->cullBackRS.Get());
            engine->context->OMSetDepthStencilState(rc->frontFaceStencilState.Get(), 1);
            engine->context->VSSetShader(rc->stencilPassShader->vertexShader.Get(), nullptr, 0);
            engine->context->PSSetShader(rc->stencilPassShader->pixelShader.Get(), nullptr, 0);
            engine->context->VSSetConstantBuffers(0, 1, rc->lightConstBuffer->buffer.GetAddressOf());
            engine->context->IASetInputLayout(rc->stencilPassShader->layout.Get());
            engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            engine->context->IASetIndexBuffer(light.aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            engine->context->IASetVertexBuffers(0, 1, light.aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            engine->context->DrawIndexed(light.aabb->indexBuffer->size, 0, 0);


            //Final Pass

            engine->context->RSSetState(rc->cullBackRS.Get());
            engine->context->OMSetDepthStencilState(rc->finalPassStencilState.Get(), 0);
            engine->context->VSSetShader(rc->dirLightShader->vertexShader.Get(), nullptr, 0);

            engine->context->PSSetShader(rc->pointLightShader->pixelShader.Get(), nullptr, 0);
            engine->context->IASetInputLayout(rc->dirLightShader->layout.Get());
            engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            engine->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            engine->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
                strides, offsets);

            engine->context->DrawIndexed(6, 0, 0);


        }

        else if (light.lightType == LightType::SpotLight)
        {
            //Back Face Pass

            engine->context->RSSetState(rc->cullFrontRS.Get());
            engine->context->OMSetDepthStencilState(rc->backFaceStencilState.Get(), 0);
            engine->context->PSSetShader(rc->stencilPassShader->pixelShader.Get(), nullptr, 0);
            engine->context->VSSetShader(rc->stencilPassShader->vertexShader.Get(), nullptr, 0);
            engine->context->IASetInputLayout(rc->stencilPassShader->layout.Get());
            engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            engine->context->IASetIndexBuffer(light.aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            engine->context->IASetVertexBuffers(0, 1, light.aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            engine->context->DrawIndexed(light.aabb->indexBuffer->size, 0, 0);


            //Front Face Pass

            engine->context->RSSetState(rc->cullBackRS.Get());
            engine->context->OMSetDepthStencilState(rc->frontFaceStencilState.Get(), 1); //change???
            engine->context->VSSetShader(rc->stencilPassShader->vertexShader.Get(), nullptr, 0);
            engine->context->PSSetShader(rc->stencilPassShader->pixelShader.Get(), nullptr, 0);
            //engine->context->PSSetShader(spotLightShader->pixelShader.Get(),nullptr,0);
            engine->context->VSSetConstantBuffers(0, 1, rc->lightConstBuffer->buffer.GetAddressOf());
            engine->context->IASetInputLayout(rc->stencilPassShader->layout.Get());
            engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            engine->context->IASetIndexBuffer(light.aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            engine->context->IASetVertexBuffers(0, 1, light.aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            engine->context->DrawIndexed(light.aabb->indexBuffer->size, 0, 0);

            //Final Pass

            engine->context->RSSetState(rc->cullBackRS.Get());
            engine->context->OMSetDepthStencilState(rc->finalPassStencilState.Get(), 0);
            engine->context->VSSetShader(rc->dirLightShader->vertexShader.Get(), nullptr, 0);
            engine->context->PSSetShader(rc->spotLightShader->pixelShader.Get(), nullptr, 0);
            engine->context->IASetInputLayout(rc->dirLightShader->layout.Get());
            engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            engine->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            engine->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
                strides, offsets);

            engine->context->DrawIndexed(6, 0, 0);

        }

    }
    engine->context->PSSetShaderResources(0, 5, srvNull);
}

void LightRenderSystem::Destroy()
{
}
