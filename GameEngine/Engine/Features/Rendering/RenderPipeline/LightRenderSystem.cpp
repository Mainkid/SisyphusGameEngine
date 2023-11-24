#include "LightRenderSystem.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../../Contexts/RenderContext.h"
#include "../../../Components/ImageBasedLightingComponent.h"
#include "../../../Scene/CameraHelper.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../Components/TransformComponent.h"
#include "../../Lighting/Components/LightComponent.h"


SyResult LightRenderSystem::Init()
{
    _hc = ServiceLocator::instance()->Get<HardwareContext>();
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    _rc = ServiceLocator::instance()->Get<RenderContext>();
 
    return SyResult();
    
}

SyResult LightRenderSystem::Run()
{

    CB_LightBuffer lightBuffer;

    
    ID3D11ShaderResourceView* resources[] = 
    { _rc->GBuffer->DiffuseSrv.Get(),
        _rc->GBuffer->MetallicSrv.Get(),
        _rc->GBuffer->DepthSrv.Get(),
        _rc->GBuffer->EmissiveSrv.Get(),
         _rc->GBuffer->NormalAndDepthSrv.Get(),
         _rc->GBuffer->PositionSrv.Get(),
         _rc->GBuffer->IdSrv.Get()
    };
    _hc->context->ClearRenderTargetView(_rc->GBuffer->HdrBufferRtv.Get(), _rc->RhData.bgColor0000);
    _hc->renderTarget->ClearRenderTarget(_hc->depthStencilView.Get(), D3D11_CLEAR_STENCIL);



    auto [camera, cameraTf] = CameraHelper::Find(_ecs);

    auto view = _ecs->view<TransformComponent, LightComponent>();
    for (auto& entity : view)
    {
        LightComponent& light = view.get<LightComponent>(entity);
        TransformComponent& tc = view.get<TransformComponent>(entity);
        lightBuffer.lightData.Pos = Vector4(tc._position.x, tc._position.y, tc._position.z, 1);
        lightBuffer.lightData.Color = light.Color;
        lightBuffer.lightData.Dir = Vector4::Transform(Vector4::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation));
        lightBuffer.lightData.additiveParams = light.ParamsRadiusAndAttenuation;
        lightBuffer.eyePos = Vector4(cameraTf._position.x, cameraTf._position.y, cameraTf._position.z, 1.0f);

        _hc->context->ClearDepthStencilView(_hc->depthStencilView.Get(), D3D11_CLEAR_STENCIL, 1, 0);

        
        

        if (light.LightType == ELightType::Directional || light.LightType == ELightType::Ambient)
        {

            

            lightBuffer.baseData.world = Matrix::Identity;
            lightBuffer.baseData.worldView = Matrix::Identity * camera.view;
            lightBuffer.baseData.worldViewProj = Matrix::Identity * camera.view * camera.projection;
            lightBuffer.baseData.worldViewInverseTranspose = Matrix::Identity;


            if (light.LightType == ELightType::Directional)
            {
                for (int i = 0; i < 4; i++)
                {
                    lightBuffer.distances[i] = light.Distances[i];
                    lightBuffer.viewProjs[i] = light.ViewMatrices[i] * light.OrthoMatrices[i];
                }
                _hc->context->ClearRenderTargetView(light.DirShadowRtv.Get(), _rc->RhData.bgColor0000);
            }


        }
        else if (light.LightType == ELightType::PointLight)
        {
            using namespace DirectX::SimpleMath;
            lightBuffer.baseData.world = Matrix::CreateScale(light.ParamsRadiusAndAttenuation.x, light.ParamsRadiusAndAttenuation.x, light.ParamsRadiusAndAttenuation.x) * Matrix::CreateTranslation(tc._position);
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
                Matrix::CreateTranslation(tc._position);
            lightBuffer.baseData.worldView = lightBuffer.baseData.world * camera.view;
            lightBuffer.baseData.worldViewProj = lightBuffer.baseData.worldView * camera.projection;
            lightBuffer.baseData.worldViewInverseTranspose = DirectX::SimpleMath::Matrix::Identity;
            
           
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT res = _hc->context->Map(_rc->LightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &lightBuffer, sizeof(CB_LightBuffer));
        _hc->context->Unmap(_rc->LightConstBuffer->buffer.Get(), 0);
        _hc->context->VSSetConstantBuffers(0, 1, _rc->LightConstBuffer->buffer.GetAddressOf());
        _hc->context->PSSetConstantBuffers(0, 1, _rc->LightConstBuffer->buffer.GetAddressOf());
        

        _hc->context->OMSetBlendState(_rc->LightBlendState.Get(), nullptr, 0xffffffff);
        _hc->context->PSSetSamplers(0, 1, _rc->SamplerState.GetAddressOf());
       

        _hc->context->OMSetRenderTargets(1, _rc->GBuffer->HdrBufferRtv.GetAddressOf(), _hc->depthStencilView.Get());
        //_hc->renderTarget->SetRenderTarget(_hc->depthStencilView.Get());
        _hc->context->PSSetShaderResources(0, 7, resources);

       

        if (light.LightType == ELightType::Ambient || light.LightType == ELightType::Directional)
        {
            auto irradianceView = _ecs->view<ImageBasedLightingComponent>();
            if (!irradianceView.empty())
            {
                ImageBasedLightingComponent& iblComp = _ecs->get<ImageBasedLightingComponent>(irradianceView.front());

                _hc->context->PSSetShaderResources(11, 1, iblComp.IrradianceCubeMapSrv.GetAddressOf());
                _hc->context->PSSetShaderResources(12, 1, iblComp.IblLookUpSrv.GetAddressOf());
                _hc->context->PSSetShaderResources(13, 1, iblComp.FilteredEnvironmentSrv.GetAddressOf());
            }

            _hc->context->PSSetSamplers(1, 1, light.ShadowMapSampler.GetAddressOf());
            _hc->context->RSSetState(_rc->CullBackRasterizerState.Get());
            _hc->context->OMSetDepthStencilState(_rc->OffStencilState.Get(), 0);
            _hc->context->VSSetShader(_rc->DirLightShader->vertexShader.Get(), nullptr, 0);
            if (light.LightType == ELightType::Directional)
            {
                _hc->context->PSSetShader(_rc->DirLightShader->pixelShader.Get(), nullptr, 0);
                _hc->context->PSSetShaderResources(8, 1, light.DirShadowSrv.GetAddressOf());
                _hc->context->PSSetShaderResources(10, 1, light.DirShadowBluredSrv.GetAddressOf());
            }
            else
            {
                _hc->context->PSSetShader(_rc->AmbientLightShader->pixelShader.Get(), nullptr, 0);
                _hc->context->PSSetShaderResources(7, 1, _rc->HbaoSrv.GetAddressOf());
            	_hc->context->PSSetShaderResources(9, 1, _rc->GBuffer->SkyboxSrv.GetAddressOf());

                
               

            }

            _hc->context->IASetInputLayout(_rc->DirLightShader->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            _hc->context->IASetIndexBuffer(_rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _hc->context->IASetVertexBuffers(0, 1, _rc->VertexQuadBuffer->buffer.GetAddressOf(),
                _rc->RhData.strides32, _rc->RhData.offsets0);
            _hc->context->DrawIndexed(6, 0, 0);
            
        }
        else if (light.LightType == ELightType::PointLight)
        {

            //Back Face Pass
            _hc->context->PSSetShaderResources(8, 1, light.ShadowCubeMapSrv.GetAddressOf());

            _hc->context->RSSetState(_rc->CullFrontRasterizerState.Get());
            _hc->context->VSSetConstantBuffers(0, 1, _rc->LightConstBuffer->buffer.GetAddressOf());
            _hc->context->OMSetDepthStencilState(_rc->BackFaceStencilState.Get(), 0);
            _hc->context->PSSetShader(_rc->StencilPassShader->pixelShader.Get(), nullptr, 0);
            _hc->context->VSSetShader(_rc->StencilPassShader->vertexShader.Get(), nullptr, 0);
            _hc->context->IASetInputLayout(_rc->StencilPassShader->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            _hc->context->IASetIndexBuffer(light.Aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _hc->context->IASetVertexBuffers(0, 1, light.Aabb->vertexBuffer->buffer.GetAddressOf(),
                _rc->RhData.strides80, _rc->RhData.offsets0);

            _hc->context->DrawIndexed(light.Aabb->indexBuffer->size, 0, 0);


            //Front Face Pass
            _hc->context->RSSetState(_rc->CullBackRasterizerState.Get());
            _hc->context->OMSetDepthStencilState(_rc->FrontFaceStencilState.Get(), 1);
            _hc->context->VSSetShader(_rc->StencilPassShader->vertexShader.Get(), nullptr, 0);
            _hc->context->PSSetShader(_rc->StencilPassShader->pixelShader.Get(), nullptr, 0);
            _hc->context->VSSetConstantBuffers(0, 1, _rc->LightConstBuffer->buffer.GetAddressOf());
            _hc->context->IASetInputLayout(_rc->StencilPassShader->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            _hc->context->IASetIndexBuffer(light.Aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _hc->context->IASetVertexBuffers(0, 1, light.Aabb->vertexBuffer->buffer.GetAddressOf(),
                _rc->RhData.strides80, _rc->RhData.offsets0);

            _hc->context->DrawIndexed(light.Aabb->indexBuffer->size, 0, 0);


            //Final Pass

            _hc->context->RSSetState(_rc->CullBackRasterizerState.Get());
            _hc->context->OMSetDepthStencilState(_rc->FinalPassStencilState.Get(), 0);
            _hc->context->VSSetShader(_rc->DirLightShader->vertexShader.Get(), nullptr, 0);

            _hc->context->PSSetShader(_rc->PointLightShader->pixelShader.Get(), nullptr, 0);
            _hc->context->IASetInputLayout(_rc->DirLightShader->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            _hc->context->IASetIndexBuffer(_rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _hc->context->IASetVertexBuffers(0, 1, _rc->VertexQuadBuffer->buffer.GetAddressOf(),
                _rc->RhData.strides32, _rc->RhData.offsets0);

            _hc->context->DrawIndexed(6, 0, 0);
        }

        else if (light.LightType == ELightType::SpotLight)
        {
            //Back Face Pass

            _hc->context->RSSetState(_rc->CullFrontRasterizerState.Get());
            _hc->context->OMSetDepthStencilState(_rc->BackFaceStencilState.Get(), 0);
            _hc->context->PSSetShader(_rc->StencilPassShader->pixelShader.Get(), nullptr, 0);
            _hc->context->VSSetShader(_rc->StencilPassShader->vertexShader.Get(), nullptr, 0);
            _hc->context->IASetInputLayout(_rc->StencilPassShader->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            _hc->context->IASetIndexBuffer(light.Aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _hc->context->IASetVertexBuffers(0, 1, light.Aabb->vertexBuffer->buffer.GetAddressOf(),
                _rc->RhData.strides80, _rc->RhData.offsets0);

            _hc->context->DrawIndexed(light.Aabb->indexBuffer->size, 0, 0);


            //Front Face Pass

            _hc->context->RSSetState(_rc->CullBackRasterizerState.Get());
            _hc->context->OMSetDepthStencilState(_rc->FrontFaceStencilState.Get(), 1); //change???
            _hc->context->VSSetShader(_rc->StencilPassShader->vertexShader.Get(), nullptr, 0);
            _hc->context->PSSetShader(_rc->StencilPassShader->pixelShader.Get(), nullptr, 0);
            //_hc->context->PSSetShader(SpotLightShader->pixelShader.Get(),nullptr,0);
            _hc->context->VSSetConstantBuffers(0, 1, _rc->LightConstBuffer->buffer.GetAddressOf());
            _hc->context->IASetInputLayout(_rc->StencilPassShader->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            _hc->context->IASetIndexBuffer(light.Aabb->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _hc->context->IASetVertexBuffers(0, 1, light.Aabb->vertexBuffer->buffer.GetAddressOf(),
                _rc->RhData.strides80, _rc->RhData.offsets0);

            _hc->context->DrawIndexed(light.Aabb->indexBuffer->size, 0, 0);

            //Final Pass
            _hc->context->RSSetState(_rc->CullBackRasterizerState.Get());
            _hc->context->OMSetDepthStencilState(_rc->FinalPassStencilState.Get(), 0);
            _hc->context->VSSetShader(_rc->DirLightShader->vertexShader.Get(), nullptr, 0);
            _hc->context->PSSetShader(_rc->SpotLightShader->pixelShader.Get(), nullptr, 0);
            _hc->context->IASetInputLayout(_rc->DirLightShader->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            _hc->context->IASetIndexBuffer(_rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _hc->context->IASetVertexBuffers(0, 1, _rc->VertexQuadBuffer->buffer.GetAddressOf(),
                _rc->RhData.strides32, _rc->RhData.offsets0);

            _hc->context->DrawIndexed(6, 0, 0);
        }
    }
    _hc->context->PSSetShaderResources(0, 13, _rc->RhData.NullSrv);
    return SyResult();
}

SyResult LightRenderSystem::Destroy()
{
    return SyResult();
}

void LightRenderSystem::ShadowMap()
{
    

}
