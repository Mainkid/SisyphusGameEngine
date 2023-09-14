#include "EditorBillboardRenderSystem.h"
#include "../../Core/EngineCore.h"
#include "../../Core/Graphics/ConstantBuffer.h"

void EditorBillboardRenderSystem::Init()
{
	engine = EngineCore::instance();
	rc = ServiceLocator::instance()->Get<RenderContext>();
}

void EditorBillboardRenderSystem::Run()
{
    ID3D11RenderTargetView* nullRTV[5] = { nullptr,nullptr,nullptr,nullptr,nullptr };
    EngineCore::instance()->context->OMSetRenderTargets(5, nullRTV, nullptr);
    engine->context->RSSetState(rc->cullBackRS.Get());
    engine->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

    UINT strides[1] = { 32 };
    UINT offsets[1] = { 0 };
    auto view = engine->scene->registry.view<TransformComponent, CameraComponent>();
    CameraComponent& ecs = engine->scene->registry.get<CameraComponent>(view.front());
    TransformComponent& camTransform = engine->scene->registry.get<TransformComponent>(view.front());

    auto view2 = engine->scene->registry.view<TransformComponent, EditorBillboardComponent>();

    engine->context->OMSetDepthStencilState(engine->depthStencilState.Get(), 0);
    engine->context->VSSetShader(rc->billboardShader->vertexShader.Get(), nullptr, 0); //
    engine->context->PSSetShader(rc->billboardShader->pixelShader.Get(), nullptr, 0); //
    engine->context->IASetInputLayout(rc->billboardShader->layout.Get());
    engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    engine->context->IASetIndexBuffer(rc->indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    engine->context->IASetVertexBuffers(0, 1, rc->vertexQuadBuffer->buffer.GetAddressOf(),
        strides, offsets);
    EngineCore::instance()->context->OMSetRenderTargets(2, rc->editorBillboardRtvs, engine->depthStencilView.Get());

    for (auto& entity : view2)
    {
        CB_BaseEditorBuffer dataOpaque;
        TransformComponent& transformComp = engine->scene->registry.get<TransformComponent>(entity);
        EditorBillboardComponent& billboardComp = engine->scene->registry.get<EditorBillboardComponent>(entity);


        Vector3 tmp = Vector3(ecs.forward.x, ecs.forward.y, ecs.forward.z);

        auto billboardMat = Matrix::CreateConstrainedBillboard(transformComp.localPosition, camTransform.localPosition,
            Vector3(ecs.up), &tmp);
        billboardMat = Matrix::CreateScale(0.25f) * billboardMat;

        dataOpaque.baseData.world = billboardMat;

        dataOpaque.baseData.worldViewProj =
            billboardMat *
            engine->scene->camera->view * engine->scene->camera->projection;

        dataOpaque.baseData.worldView = billboardMat *
            engine->scene->camera->view;

        dataOpaque.baseData.worldViewInverseTranspose =
            DirectX::XMMatrixTranspose(
                DirectX::XMMatrixInverse(nullptr,
                    billboardMat));

        dataOpaque.instanseID = (uint32_t)entity;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT res = engine->context->Map(rc->opaqueConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_BaseEditorBuffer));
        engine->context->Unmap(rc->opaqueConstBuffer->buffer.Get(), 0);
        engine->context->VSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());
        engine->context->PSSetConstantBuffers(0, 1, rc->opaqueConstBuffer->buffer.GetAddressOf());
        //engine->renderTarget->SetRenderTarget(engine->depthStencilView.Get());

        engine->context->PSSetSamplers(0, 1, billboardComp.samplerState.GetAddressOf());

        engine->context->PSSetShaderResources(0, 1, billboardComp.texture.GetAddressOf());
        engine->context->DrawIndexed(6, 0, 0);

    }
    ID3D11ShaderResourceView* srvNull = nullptr;
    engine->context->PSSetShaderResources(0, 1, &srvNull);
    EngineCore::instance()->context->OMSetRenderTargets(2, nullRTV, nullptr);

}

void EditorBillboardRenderSystem::Destroy()
{
}
