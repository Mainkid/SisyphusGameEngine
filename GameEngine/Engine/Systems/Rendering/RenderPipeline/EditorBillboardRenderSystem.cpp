#include "EditorBillboardRenderSystem.h"
#include "../RenderContext.h"
#include "../RenderHelper.h"
#include "../../HardwareContext.h"
#include "../../EngineContext.h"
#include "../../../Scene/CameraHelper.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/CameraComponent.h"
#include "../../Components/EditorBillboardComponent.h"

SyResult EditorBillboardRenderSystem::Init()
{
    _hc = ServiceLocator::instance()->Get<HardwareContext>();
    _rc = ServiceLocator::instance()->Get<RenderContext>();
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    return SyResult();

}

SyResult EditorBillboardRenderSystem::Run()
{

    _hc->context->OMSetRenderTargets(5, _rc->RhData.NullRtv, nullptr);
    _hc->context->RSSetState(_rc->CullBackRasterizerState.Get());
    _hc->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

    auto [camera, cameraTf] = CameraHelper::Find(_ecs);

    auto viewBb = _ecs->view<TransformComponent, EditorBillboardComponent>();

    _hc->context->OMSetDepthStencilState(_hc->depthStencilState.Get(), 0);
    _hc->context->VSSetShader(_rc->BillboardShader->vertexShader.Get(), nullptr, 0); //
    _hc->context->PSSetShader(_rc->BillboardShader->pixelShader.Get(), nullptr, 0); //
    _hc->context->IASetInputLayout(_rc->BillboardShader->layout.Get());
    _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _hc->context->IASetIndexBuffer(_rc->IndexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    _hc->context->IASetVertexBuffers(0, 1, _rc->VertexQuadBuffer->buffer.GetAddressOf(),
        _rc->RhData.strides32 , _rc->RhData.offsets0);
    _hc->context->OMSetRenderTargets(2, _rc->EditorBillboardRtvs, _hc->depthStencilView.Get());

    for (auto& ent : viewBb)
    {
        CB_BaseEditorBuffer dataOpaque;
        TransformComponent& transformComp = viewBb.get<TransformComponent>(ent);
        EditorBillboardComponent& billboardComp = viewBb.get<EditorBillboardComponent>(ent);

        Vector3 tmp = Vector3(camera.forward.x, camera.forward.y, camera.forward.z);

        auto billboardMat = Matrix::CreateConstrainedBillboard(transformComp._position, cameraTf._position,
            Vector3(camera.up), &tmp);
        billboardMat = Matrix::CreateScale(0.25f) * billboardMat;

        dataOpaque.baseData.world = billboardMat;

        dataOpaque.baseData.worldViewProj = billboardMat * camera.view * camera.projection;

        dataOpaque.baseData.worldView = billboardMat * camera.view;

        dataOpaque.baseData.worldViewInverseTranspose =
            DirectX::XMMatrixTranspose(
                DirectX::XMMatrixInverse(nullptr,
                    billboardMat));

        dataOpaque.instanseID = DirectX::SimpleMath::Vector4(int(ent), int(ent), int(EAssetType::ASSET_TEXTURE), 1);

        D3D11_MAPPED_SUBRESOURCE mappedResou_rce;
        HRESULT res = _hc->context->Map(_rc->OpaqueConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResou_rce);
        CopyMemory(mappedResou_rce.pData, &dataOpaque, sizeof(CB_BaseEditorBuffer));
        _hc->context->Unmap(_rc->OpaqueConstBuffer->buffer.Get(), 0);
        _hc->context->VSSetConstantBuffers(0, 1, _rc->OpaqueConstBuffer->buffer.GetAddressOf());
        _hc->context->PSSetConstantBuffers(0, 1, _rc->OpaqueConstBuffer->buffer.GetAddressOf());
        //engine->renderTarget->SetRenderTarget(engine->depthStencilView.Get());

        _hc->context->PSSetSamplers(0, 1, billboardComp.samplerState.GetAddressOf());
        _hc->context->PSSetShaderResources(0, 1, billboardComp.texture.GetAddressOf());
        _hc->context->DrawIndexed(6, 0, 0);

    }
    ID3D11ShaderResourceView* srvNull = nullptr;
    _hc->context->PSSetShaderResources(0, 1, &srvNull);
    _hc->context->OMSetRenderTargets(2, _rc->RhData.NullRtv, nullptr);
    return SyResult();
}

SyResult EditorBillboardRenderSystem::Destroy()
{
    return SyResult();
}
