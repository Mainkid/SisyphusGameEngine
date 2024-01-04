#include "SkyboxRenderSystem.h"
#include "../../../Contexts/RenderContext.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../../Components/CameraComponent.h"
#include "../../../Scene/CameraHelper.h"
#include "../../Core/ServiceLocator.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "..\..\..\Components\SkyboxComponent.h"

SyResult SkyboxRenderSystem::Init()
{
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    _rc = ServiceLocator::instance()->Get<RenderContext>();
    _hc = ServiceLocator::instance()->Get<HardwareContext>();
    _rs = ServiceLocator::instance()->Get<ResourceService>();
    return SyResult();
}

SyResult SkyboxRenderSystem::Run()
{
    OPTICK_EVENT();
    if (_ecs->view<SkyboxComponent>().size() == 0)
        return SyResult();
    //_hc->context->OMSetDepthStencilState(_rc->OffStencilState.Get(), 0);
    entt::entity skyboxEnt = _ecs->view<SkyboxComponent>().front();
    SkyboxComponent& skybox = _ecs->get<SkyboxComponent>(skyboxEnt);


    _hc->context->RSSetState(_rc->CullFrontRasterizerState.Get());
    CB_BaseEditorBuffer dataOpaque;
    
    auto [camera, cameraTf] = CameraHelper::Find(_ecs,_ec->playModeState);

    dataOpaque.baseData.world = camera.view * camera.projection;

   

    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT res = _hc->context->Map(_rc->OpaqueConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_BaseEditorBuffer));
    _hc->context->Unmap(_rc->OpaqueConstBuffer->buffer.Get(), 0);
    _hc->context->VSSetConstantBuffers(0, 1, _rc->OpaqueConstBuffer->buffer.GetAddressOf());
    _hc->context->PSSetConstantBuffers(0, 1, _rc->OpaqueConstBuffer->buffer.GetAddressOf());
    _hc->context->ClearRenderTargetView(_rc->GBuffer->SkyboxRtv.Get(), _rc->RhData.bgColor0000);
    _hc->context->OMSetDepthStencilState(nullptr, 0);
    _hc->context->OMSetRenderTargets(1,_rc->GBuffer->HdrBufferRtv.GetAddressOf(), _hc->depthStencilView.Get());
    _hc->context->IASetInputLayout(_rc->SkyBoxShader->layout.Get());
    _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _hc->context->IASetIndexBuffer(_rc->CubeMesh->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    _hc->context->IASetVertexBuffers(0, 1, _rc->CubeMesh->vertexBuffer->buffer.GetAddressOf(),
       _rc->RhData.strides112, _rc->RhData.offsets0);
    _hc->context->PSSetSamplers(0, 1, _rc->SamplerState.GetAddressOf());
    _hc->context->PSSetShaderResources(0, 1,skybox.SkyboxRes->textureSRV.GetAddressOf());
    _hc->context->VSSetShader(_rc->SkyBoxShader->vertexShader.Get(), nullptr, 0);
    _hc->context->PSSetShader(_rc->SkyBoxShader->pixelShader.Get(), nullptr, 0);
    
    _hc->context->DrawIndexed(_rc->CubeMesh->indexBuffer->size, 0, 0);
    _hc->context->RSSetState(_rc->CullBackRasterizerState.Get());
    return SyResult();
}

SyResult SkyboxRenderSystem::Destroy()
{
    return SyResult();
}
