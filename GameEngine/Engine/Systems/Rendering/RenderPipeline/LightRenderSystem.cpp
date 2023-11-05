#include "LightRenderSystem.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../RenderContext.h"
#include "../../../Scene/CameraHelper.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/LightComponent.h"


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
    ID3D11ShaderResourceView* srvNull[] = { nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };

    ID3D11ShaderResourceView* resources[] = 
    { rc->GBuffer->DiffuseSrv.Get(),
        rc->GBuffer->MetallicSrv.Get(),
        rc->GBuffer->HbaoSrv.Get(),
        rc->GBuffer->EmissiveSrv.Get(),
         rc->GBuffer->NormalSrv.Get(),
         rc->GBuffer->PositionSrv.Get(),
         rc->GBuffer->IdSrv.Get()
    };
    hc->context->ClearRenderTargetView(rc->GBuffer->HdrBufferRtv.Get(), bgColor);
    hc->renderTarget->ClearRenderTarget(hc->depthStencilView.Get(), D3D11_CLEAR_STENCIL);
    UINT strides[1] = { 32 };
    UINT offsets[1] = { 0 };

    UINT stridesLight[1] = { 80 };
    UINT offsetsLight[1] = { 0 };

    auto [camera, cameraTf] = CameraHelper::Find(_ecs);

    auto view = _ecs->view<TransformComponent, LightComponent>();
    for (auto& entity : view)
    {
        LightComponent& light = view.get<LightComponent>(entity);
        TransformComponent& tc = view.get<TransformComponent>(entity);
        lightBuffer.lightData.Pos = Vector4(tc.position.x, tc.position.y, tc.position.z, 1);
        lightBuffer.lightData.Color = light.Color;
        lightBuffer.lightData.Dir = Vector4::Transform(Vector4::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation));
        lightBuffer.lightData.additiveParams = light.ParamsRadiusAndAttenuation;
        lightBuffer.eyePos = Vector4(cameraTf.position.x, cameraTf.position.y, cameraTf.position.z, 1.0f);

        hc->context->ClearDepthStencilView(hc->depthStencilView.Get(), D3D11_CLEAR_STENCIL, 1, 0);

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
            using namespace DirectX::SimpleMath;
            lightBuffer.baseData.world = Matrix::CreateScale(light.ParamsRadiusAndAttenuation.x, light.ParamsRadiusAndAttenuation.x, light.ParamsRadiusAndAttenuation.x) * Matrix::CreateTranslation(tc.position);
            lightBuffer.baseData.worldView = lightBuffer.baseData.world * camera.view;
            lightBuffer.baseData.worldViewProj = lightBuffer.baseData.worldView * camera.projection;
            lightBuffer.baseData.worldViewInverseTranspose = DirectX::SimpleMath::Matrix::Identity;

            lightBuffer.distances[0].x = light.CastShadows;
        }
        else if (light.LightType == ELightType::SpotLight)
        {
            using namespace DirectX::SimpleMath;
            Vector3 a;
            Vector3 forward = Vector3(0, 0, 1);
            Vector3 dir = Vector3::Transform(Vector3::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation));
            dir.Normalize();
            forward.Cross(dir, a);
            a.Normalize();
            float angle = acos(forward.Dot(dir));
            float radius = sqrt(pow((light.ParamsRadiusAndAttenuation.x / light.ParamsRadiusAndAttenuation.y), 2.0f) - pow(light.ParamsRadiusAndAttenuation.x, 2.0f));

            lightBuffer.baseData.world = Matrix::CreateTranslation(Vector3(0,
                0,
                1.0f)) *
                Matrix::CreateScale(radius, radius, light.ParamsRadiusAndAttenuation.x) *
                Matrix::CreateFromAxisAngle(a, angle) *
                Matrix::CreateTranslation(tc.position);
            lightBuffer.baseData.worldView = lightBuffer.baseData.world * camera.view;
            lightBuffer.baseData.worldViewProj = lightBuffer.baseData.worldView * camera.projection;
            lightBuffer.baseData.worldViewInverseTranspose = DirectX::SimpleMath::Matrix::Identity;
            
           
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT res = hc->context->Map(rc->LightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &lightBuffer, sizeof(CB_LightBuffer));
        hc->context->Unmap(rc->LightConstBuffer->buffer.Get(), 0);
        hc->context->VSSetConstantBuffers(0, 1, rc->LightConstBuffer->buffer.GetAddressOf());
        hc->context->PSSetConstantBuffers(0, 1, rc->LightConstBuffer->buffer.GetAddressOf());


        hc->context->OMSetBlendState(rc->LightBlendState.Get(), nullptr, 0xffffffff);
        hc->context->PSSetSamplers(0, 1, rc->SamplerState.GetAddressOf());
       

        hc->context->OMSetRenderTargets(1, rc->GBuffer->HdrBufferRtv.GetAddressOf(), hc->depthStencilView.Get());
        //hc->renderTarget->SetRenderTarget(hc->depthStencilView.Get());
        hc->context->PSSetShaderResources(0, 7, resources);

        if (light.LightType == ELightType::Ambient || light.LightType == ELightType::Directional)
        {
            hc->context->PSSetSamplers(1, 1, light.ShadowMapSampler.GetAddressOf());
            hc->context->RSSetState(rc->CullBackRasterizerState.Get());
            hc->context->OMSetDepthStencilState(rc->OffStencilState.Get(), 0);
            hc->context->VSSetShader(rc->DirLightShader->vertexShader.Get(), nullptr, 0);
            if (light.LightType == ELightType::Directional)
            {
                hc->context->PSSetShader(rc->DirLightShader->pixelShader.Get(), nullptr, 0);
                hc->context->PSSetShaderResources(8, 1, light.DirShadowSrv.GetAddressOf());
                hc->context->PSSetShaderResources(10, 1, light.DirShadowBluredSrv.GetAddressOf());
            }
            else
            {
                hc->context->PSSetShader(rc->AmbientLightShader->pixelShader.Get(), nullptr, 0);
                hc->context->PSSetShaderResources(9, 1, rc->GBuffer->SkyboxSrv.GetAddressOf());
                //hc->context->PSSetShaderResources(7, 1, hc->depthStencilView.GetAddressOf());
            }

            hc->context->IASetInputLayout(rc->DirLightShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, rc->VertexQuadBuffer->buffer.GetAddressOf(),
                strides, offsets);
            hc->context->DrawIndexed(6, 0, 0);
        }
        else if (light.LightType == ELightType::PointLight)
        {

            //Back Face Pass
            hc->context->PSSetShaderResources(8, 1, light.ShadowCubeMapSrv.GetAddressOf());

            hc->context->RSSetState(rc->CullFrontRasterizerState.Get());
            hc->context->VSSetConstantBuffers(0, 1, rc->LightConstBuffer->buffer.GetAddressOf());
            hc->context->OMSetDepthStencilState(rc->BackFaceStencilState.Get(), 0);
            hc->context->PSSetShader(rc->StencilPassShader->pixelShader.Get(), nullptr, 0);
            hc->context->VSSetShader(rc->StencilPassShader->vertexShader.Get(), nullptr, 0);
            hc->context->IASetInputLayout(rc->StencilPassShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(light.Aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, light.Aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            hc->context->DrawIndexed(light.Aabb->indexBuffer->size, 0, 0);


            //Front Face Pass
            hc->context->RSSetState(rc->CullBackRasterizerState.Get());
            hc->context->OMSetDepthStencilState(rc->FrontFaceStencilState.Get(), 1);
            hc->context->VSSetShader(rc->StencilPassShader->vertexShader.Get(), nullptr, 0);
            hc->context->PSSetShader(rc->StencilPassShader->pixelShader.Get(), nullptr, 0);
            hc->context->VSSetConstantBuffers(0, 1, rc->LightConstBuffer->buffer.GetAddressOf());
            hc->context->IASetInputLayout(rc->StencilPassShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(light.Aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, light.Aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            hc->context->DrawIndexed(light.Aabb->indexBuffer->size, 0, 0);


            //Final Pass

            hc->context->RSSetState(rc->CullBackRasterizerState.Get());
            hc->context->OMSetDepthStencilState(rc->FinalPassStencilState.Get(), 0);
            hc->context->VSSetShader(rc->DirLightShader->vertexShader.Get(), nullptr, 0);

            hc->context->PSSetShader(rc->PointLightShader->pixelShader.Get(), nullptr, 0);
            hc->context->IASetInputLayout(rc->DirLightShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, rc->VertexQuadBuffer->buffer.GetAddressOf(),
                strides, offsets);

            hc->context->DrawIndexed(6, 0, 0);
        }

        else if (light.LightType == ELightType::SpotLight)
        {
            //Back Face Pass

            hc->context->RSSetState(rc->CullFrontRasterizerState.Get());
            hc->context->OMSetDepthStencilState(rc->BackFaceStencilState.Get(), 0);
            hc->context->PSSetShader(rc->StencilPassShader->pixelShader.Get(), nullptr, 0);
            hc->context->VSSetShader(rc->StencilPassShader->vertexShader.Get(), nullptr, 0);
            hc->context->IASetInputLayout(rc->StencilPassShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(light.Aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, light.Aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            hc->context->DrawIndexed(light.Aabb->indexBuffer->size, 0, 0);


            //Front Face Pass

            hc->context->RSSetState(rc->CullBackRasterizerState.Get());
            hc->context->OMSetDepthStencilState(rc->FrontFaceStencilState.Get(), 1); //change???
            hc->context->VSSetShader(rc->StencilPassShader->vertexShader.Get(), nullptr, 0);
            hc->context->PSSetShader(rc->StencilPassShader->pixelShader.Get(), nullptr, 0);
            //hc->context->PSSetShader(SpotLightShader->pixelShader.Get(),nullptr,0);
            hc->context->VSSetConstantBuffers(0, 1, rc->LightConstBuffer->buffer.GetAddressOf());
            hc->context->IASetInputLayout(rc->StencilPassShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(light.Aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, light.Aabb->vertexBuffer->buffer.GetAddressOf(),
                stridesLight, offsetsLight);

            hc->context->DrawIndexed(light.Aabb->indexBuffer->size, 0, 0);

            //Final Pass
            hc->context->RSSetState(rc->CullBackRasterizerState.Get());
            hc->context->OMSetDepthStencilState(rc->FinalPassStencilState.Get(), 0);
            hc->context->VSSetShader(rc->DirLightShader->vertexShader.Get(), nullptr, 0);
            hc->context->PSSetShader(rc->SpotLightShader->pixelShader.Get(), nullptr, 0);
            hc->context->IASetInputLayout(rc->DirLightShader->layout.Get());
            hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            hc->context->IASetIndexBuffer(rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            hc->context->IASetVertexBuffers(0, 1, rc->VertexQuadBuffer->buffer.GetAddressOf(),
                strides, offsets);

            hc->context->DrawIndexed(6, 0, 0);
        }
    }
    hc->context->PSSetShaderResources(0, 10, srvNull);
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
