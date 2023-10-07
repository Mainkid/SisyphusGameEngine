#include "EditorBillboardRenderSystem.h"
#include "../RenderContext.h"
#include "../../HardwareContext.h"
#include "../../EngineContext.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/CameraComponent.h"
#include "../../Components/EditorBillboardComponent.h"

SyResult EditorBillboardRenderSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
    ec = ServiceLocator::instance()->Get<EngineContext>();
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "EditorBillboardRender system initialization successful.");
    return SyResult();

}

SyResult EditorBillboardRenderSystem::Run()
{
    ID3D11RenderTargetView* nullRTV[5] = { nullptr,nullptr,nullptr,nullptr,nullptr };
    hc->context->OMSetRenderTargets(5, nullRTV, nullptr);
    hc->context->RSSetState(rc->cullBackRS.Get());
    hc->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    UINT strides[1] = { 32 };
    UINT offsets[1] = { 0 };
    auto view = ec->scene->registry.view<TransformComponent, CameraComponent>();
    CameraComponent& ecs = ec->scene->registry.get<CameraComponent>(view.front());
    TransformComponent& camTransform = ec->scene->registry.get<TransformComponent>(view.front());

    auto view2 = ec->scene->registry.view<TransformComponent, EditorBillboardComponent>();

    hc->context->OMSetDepthStencilState(hc->depthStencilState.Get(), 0);
    hc->context->VSSetShader(rc->billboardShader->vertexShader.Get(), nullptr, 0); //
    hc->context->PSSetShader(rc->billboardShader->pixelShader.Get(), nullptr, 0); //
    hc->context->IASetInputLayout(rc->billboardShader->layout.Get());
    hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    hc->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    hc->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
        strides, offsets);
    hc->context->OMSetRenderTargets(2, rc->editorBillboardRtvs, hc->depthStencilView.Get());

    for (auto& entity : view2)
    {
        CB_BaseEditorBuffer dataOpaque;
        TransformComponent& transformComp = ec->scene->registry.get<TransformComponent>(entity);
        EditorBillboardComponent& billboardComp = ec->scene->registry.get<EditorBillboardComponent>(entity);


        Vector3 tmp = Vector3(ecs.forward.x, ecs.forward.y, ecs.forward.z);

        auto billboardMat = Matrix::CreateConstrainedBillboard(transformComp.position, camTransform.position,
            Vector3(ecs.up), &tmp);
        billboardMat = Matrix::CreateScale(0.25f) * billboardMat;

        dataOpaque.baseData.world = billboardMat;

        dataOpaque.baseData.worldViewProj =
            billboardMat *
            ec->scene->camera->view * ec->scene->camera->projection;

        dataOpaque.baseData.worldView = billboardMat *
            ec->scene->camera->view;

        dataOpaque.baseData.worldViewInverseTranspose =
            DirectX::XMMatrixTranspose(
                DirectX::XMMatrixInverse(nullptr,
                    billboardMat));

        dataOpaque.instanseID = (uint32_t)entity;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT res = hc->context->Map(rc->opaqueConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_BaseEditorBuffer));
        hc->context->Unmap(rc->opaqueConstBuffer->buffer.Get(), 0);
        hc->context->VSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());
        hc->context->PSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());
        //engine->renderTarget->SetRenderTarget(engine->depthStencilView.Get());

        hc->context->PSSetSamplers(0, 1, billboardComp.samplerState.GetAddressOf());
        hc->context->PSSetShaderResources(0, 1, billboardComp.texture.GetAddressOf());
        hc->context->DrawIndexed(6, 0, 0);

    }
    ID3D11ShaderResourceView* srvNull = nullptr;
    hc->context->PSSetShaderResources(0, 1, &srvNull);
    hc->context->OMSetRenderTargets(2, nullRTV, nullptr);
    return SyResult();
}

SyResult EditorBillboardRenderSystem::Destroy()
{

    SY_LOG_CORE(SY_LOGLEVEL_INFO, "EditorBillboardRender system destruction successful.");
    return SyResult();
}
