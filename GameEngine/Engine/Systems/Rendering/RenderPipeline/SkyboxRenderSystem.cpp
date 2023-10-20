#include "SkyboxRenderSystem.h"
#include "../RenderContext.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../../../Components/CameraComponent.h"
#include "../../../Scene/CameraHelper.h"
#include "../../Core/ServiceLocator.h"
#include "../../Core/Graphics/ConstantBuffer.h"

SyResult SkyboxRenderSystem::Init()
{
    ec = ServiceLocator::instance()->Get<EngineContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "SkyboxRender system initialization successful. ");
    return SyResult();
}

SyResult SkyboxRenderSystem::Run()
{
    float bgcolor[] = { 0.0f,0.0f,0.0f,0.0f };
    hc->context->RSSetState(rc->cullFrontRS.Get());
    //hc->context->ClearDepthStencilView(rc->shadowStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    CB_BaseEditorBuffer dataOpaque;
    //dataOpaque.world = engineActor->transform->world * engineActor->transform->GetViewMatrix();

    auto [camera, cameraTf] = CameraHelper::Find(_ecs);

    dataOpaque.baseData.world = camera.view * camera.projection;

    UINT strides[1] = {80 };
    UINT offsets[1] = { 0 };
    ID3D11RenderTargetView* nulls[] = { nullptr };
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT res = hc->context->Map(rc->opaqueConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_BaseEditorBuffer));
    hc->context->Unmap(rc->opaqueConstBuffer->buffer.Get(), 0);
    hc->context->VSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());
    hc->context->PSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());
    hc->context->ClearRenderTargetView(rc->gBuffer->skyboxRTV.Get(), bgcolor);
    hc->context->OMSetRenderTargets(1,rc->gBuffer->skyboxRTV.GetAddressOf(), nullptr);
    hc->context->IASetInputLayout(rc->skyBoxShader->layout.Get());
    hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    hc->context->IASetIndexBuffer(rc->cubeMesh->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    hc->context->IASetVertexBuffers(0, 1, rc->cubeMesh->vertexBuffer->buffer.GetAddressOf(),
       strides, offsets);
    hc->context->PSSetSamplers(0, 1, rc->samplerState.GetAddressOf());
    hc->context->PSSetShaderResources(0, 1, rc->skyboxSRV.GetAddressOf());
    hc->context->VSSetShader(rc->skyBoxShader->vertexShader.Get(), nullptr, 0);
    hc->context->PSSetShader(rc->skyBoxShader->pixelShader.Get(), nullptr, 0);
    
    hc->context->DrawIndexed(rc->cubeMesh->indexBuffer->size, 0, 0);
    
    return SyResult();
}

SyResult SkyboxRenderSystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "SkyboxRender system destruction successful. ");
    return SyResult();
}
