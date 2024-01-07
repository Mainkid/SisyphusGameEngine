#include "NavMeshDrawSystem.h"


#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../../Contexts/RenderContext.h"
#include "../../../Core/Tools/Macro.h"
#include "../../../Features/AI/Components/NavMeshComponent.h"
#include "../../../Core/Tools/Structures/Geometry.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../../Scene/CameraHelper.h"

#define CHECK_RESULT(messageText, ...)    if (result.code == SY_RESCODE_ERROR) \
                                                       {\
                                                            SY_LOG_AI(SY_LOGLEVEL_ERROR, messageText, __VA_ARGS__); \
                                                            continue; \
                                                       } \
                                                    
SyResult SyNavMeshDrawSystem::Init()
{
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    _rc = ServiceLocator::instance()->Get<RenderContext>();
    _hc = ServiceLocator::instance()->Get<HardwareContext>();
    return SyResult();
}

SyResult SyNavMeshDrawSystem::Run()
{
    if (ServiceLocator::instance()->Get<EngineContext>()->playModeState != EngineContext::EPlayModeState::PlayMode)
        return SyResult();
    SyResult result;
    static bool wasNavMeshDrawn = false;
    if (wasNavMeshDrawn == false)
    {
        auto view = _ecs->view<SyNavMeshComponent>();
        for (auto& entity : view)
        {
            auto& navMeshC = _ecs->get<SyNavMeshComponent>(entity);
            auto entityName = SY_GET_ENTITY_NAME_CHAR(_ecs, entity);
            result = PrepareRenderDataAabb(entity, navMeshC);
            result = PrepareRenderDataMesh(entity, navMeshC);
            CHECK_RESULT("Failed to prepare mesh render data for NavMesh Component on entity (%s)", entityName);
            result = PrepareRenderDataInternalEdges(entity, navMeshC);
            CHECK_RESULT("Failed to prepare internal edges render data for NavMesh Component on entity (%s)", entityName);
            result = PrepareRenderDataExternalEdges(entity, navMeshC);
            CHECK_RESULT("Failed to prepare external edges render data for NavMesh Component on entity (%s)", entityName);
        }
        wasNavMeshDrawn = true;
    }
    else
    {
        CB_GridEditorBuffer dataOpaque;

        _hc->context->OMSetBlendState(_rc->GridBlendState.Get(), nullptr, 0xffffffff);
        auto [camera, cameraTransform] = CameraHelper::Find(_ecs, _ec->playModeState);

        dataOpaque.baseData.worldViewProj = camera.view * camera.projection;
        _hc->context->OMSetDepthStencilState(_rc->OffStencilState.Get(), 0);





        auto view = _ecs->view<SyNavMeshComponent>();
        for (auto& ent : view)
        {
           /* if ((_ec->propertyFlags & SyEEnginePropertiesFlags::SHOW_ALL_COLLIDERS) == 0
                && !_ec->hudData.selectedEntityIDs.contains(ent))
                continue;*/

            auto& transform = _ecs->get<TransformComponent>(ent);
            auto& navMesh = _ecs->get<SyNavMeshComponent>(ent);
            

            dataOpaque.baseData.world = Matrix::CreateScale(Vector3(1.002f, 1.002f, 1.002f)) * Matrix::CreateFromYawPitchRoll(transform.localRotation) * Matrix::CreateTranslation(transform.localPosition);
            D3D11_MAPPED_SUBRESOURCE mappedResource;

            HRESULT res = _hc->context->Map(_rc->ShadowConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0,
                &mappedResource);
            CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_GridEditorBuffer));
            _hc->context->Unmap(_rc->ShadowConstBuffer->buffer.Get(), 0);

            _hc->context->VSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());
            _hc->context->PSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());
            _hc->renderTarget->SetRenderTarget(_hc->depthStencilView.Get());
            _hc->context->IASetInputLayout(_rc->EditorColliderRenderer->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
            _hc->context->IASetIndexBuffer(navMesh._aabbIndexBuffer.get()->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _hc->context->IASetVertexBuffers(0, 1, navMesh._aabbVertexBuffer.get()->buffer.GetAddressOf(),
                _rc->RhData.strides16, _rc->RhData.offsets0);


            _hc->context->VSSetShader(_rc->EditorColliderRenderer->vertexShader.Get(), nullptr, 0);
            _hc->context->PSSetShader(_rc->EditorColliderRenderer->pixelShader.Get(), nullptr, 0);
            _hc->context->DrawIndexed(navMesh._aabbIndexBuffer.get()->size, 0, 0);


            //---- Mesh Rendering ----

            _hc->context->VSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());
            _hc->context->PSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());
            _hc->renderTarget->SetRenderTarget(_hc->depthStencilView.Get());
            _hc->context->IASetInputLayout(_rc->NavMeshShader->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


            //_hc->context->IASetIndexBuffer(navMesh._meshVertexBuffer.get()->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            _hc->context->IASetVertexBuffers(0, 1, navMesh._meshVertexBuffer.get()->buffer.GetAddressOf(),
                _rc->RhData.strides32, _rc->RhData.offsets0);


            _hc->context->VSSetShader(_rc->NavMeshShader->vertexShader.Get(), nullptr, 0);
            _hc->context->PSSetShader(_rc->NavMeshShader->pixelShader.Get(), nullptr, 0);
            _hc->context->Draw(navMesh._meshVertexBuffer.get()->size/(sizeof(Vector4)*2), 0);
            
            //------External Edges-----

            _hc->context->VSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());
            _hc->context->PSSetConstantBuffers(0, 1, _rc->ShadowConstBuffer->buffer.GetAddressOf());
            _hc->renderTarget->SetRenderTarget(_hc->depthStencilView.Get());
            _hc->context->IASetInputLayout(_rc->LineShader->layout.Get());
            _hc->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

            _hc->context->IASetVertexBuffers(0, 1, navMesh._externalEdgesVertexBuffer.get()->buffer.GetAddressOf(),
                _rc->RhData.strides32, _rc->RhData.offsets0);


            _hc->context->VSSetShader(_rc->LineShader->vertexShader.Get(), nullptr, 0);
            _hc->context->PSSetShader(_rc->LineShader->pixelShader.Get(), nullptr, 0);

            _hc->context->Draw(navMesh._externalEdgesVertexBuffer.get()->size / (sizeof(Vector4) * 2), 0);

        }


    }


    return SyResult();
}

SyResult SyNavMeshDrawSystem::Destroy()
{
    return SyResult();
}

SyResult SyNavMeshDrawSystem::PrepareRenderDataAabb(const entt::entity& entity, SyNavMeshComponent& navMeshC)
{
    SyVector3 tmpVec;

    std::vector<Vector4> vertices(8);
    auto e = navMeshC.extent;
    auto o = navMeshC.origin;
    tmpVec = o + e.ReflectX().ReflectZ();
    vertices[0] = Vector4(tmpVec.x,tmpVec.y,tmpVec.z, 1);

    tmpVec = o + e.ReflectX();
    vertices[1] = Vector4(tmpVec.x, tmpVec.y, tmpVec.z, 1);

    tmpVec = o + e;
    vertices[2] = Vector4(tmpVec.x, tmpVec.y, tmpVec.z, 1);

    tmpVec = o + e.ReflectZ();
    vertices[3] = Vector4(tmpVec.x, tmpVec.y, tmpVec.z, 1);

    tmpVec = o + e.ReflectX().ReflectZ().ReflectY();
    vertices[4] = Vector4(tmpVec.x, tmpVec.y, tmpVec.z, 1);

    tmpVec = o + e.ReflectX().ReflectY();
    vertices[5] = Vector4(tmpVec.x, tmpVec.y, tmpVec.z, 1);

    tmpVec = o + e.ReflectY();
    vertices[6] = Vector4(tmpVec.x, tmpVec.y, tmpVec.z, 1);

    tmpVec = o + e.ReflectZ().ReflectY();
    vertices[7] = Vector4(tmpVec.x, tmpVec.y, tmpVec.z, 1);
    std::vector<int> indices = {0,1,1,2,2,3,3,0, 4,5,5,6,6,7,7,4, 0,4, 1,5, 2,6, 3,7};
    navMeshC._aabbVertexBuffer->Initialize(vertices);
    navMeshC._aabbIndexBuffer->Initialize(indices);
    return SyResult();    
}

SyResult SyNavMeshDrawSystem::PrepareRenderDataMesh(const entt::entity& entity, SyNavMeshComponent& navMeshC)
{
    SyResult result;
    std::vector<Vector4> vertices;
    std::vector<Vector4> colors;
    if (navMeshC.meshDetail.get() == nullptr)
    {
        SY_LOG_AI(SY_LOGLEVEL_ERROR, "No NavMesh detail found on entity (%s)", SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
        navMeshC._wasMeshDataPrepared = false;
        result.code = SY_RESCODE_ERROR;
        result.message = xstring("No NavMesh detail found on entity (%s)", SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
        return result;
    }
    const auto& dmesh = *navMeshC.meshDetail.get();
    for (int i = 0; i < dmesh.nmeshes; ++i)
    {
        const unsigned int* m = &dmesh.meshes[i*4];
        const unsigned int bverts = m[0];
        const unsigned int btris = m[2];
        const int ntris = (int)m[3];
        const float* verts = &dmesh.verts[bverts*3];
        const unsigned char* tris = &dmesh.tris[btris*4];
        Vector4 color = Vector4(IntToColor(i).x, IntToColor(i).y, IntToColor(i).z, 1.0f);
        Vector4 tmp;
        SyVector3 tmpSy;
        for (int j = 0; j < ntris; ++j)
        {
            tmpSy = &verts[tris[j * 4 + 0] * 3];
            tmp = Vector4(tmpSy.x, tmpSy.y, tmpSy.z + 0.01f, 1);
            vertices.push_back(tmp);
            vertices.push_back(color);

            tmpSy = &verts[tris[j * 4 + 1] * 3];
            tmp = Vector4(tmpSy.x, tmpSy.y, tmpSy.z + 0.01f, 1);
            vertices.push_back(tmp);
            vertices.push_back(color);

            tmpSy = &verts[tris[j * 4 + 2] * 3];
            tmp = Vector4(tmpSy.x, tmpSy.y, tmpSy.z + 0.01f, 1);
            vertices.push_back(tmp);
            vertices.push_back(color);

            //colors.insert(colors.end(), 3, color);
        }
    }
    //!!!
    navMeshC._meshVertexBuffer->Initialize(vertices);
    navMeshC._wasMeshDataPrepared = true;
    return SyResult();
}

SyResult SyNavMeshDrawSystem::PrepareRenderDataInternalEdges(const entt::entity& entity, SyNavMeshComponent& navMeshC)
{
    SyResult result;
    std::vector<SyVector3> vertices;
    if (navMeshC.meshDetail.get() == nullptr)
    {
        SY_LOG_AI(SY_LOGLEVEL_ERROR, "No NavMesh detail found on entity (%s)", SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
        navMeshC._wasIntEdgesDataPrepared = false;
        result.code = SY_RESCODE_ERROR;
        result.message = xstring("No NavMesh detail found on entity (%s)", SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
        return result;
    }
    const auto& dmesh = *navMeshC.meshDetail.get();
    for (int i = 0; i < dmesh.nmeshes; ++i)
    {
        const unsigned int* m = &dmesh.meshes[i*4];
        const unsigned int bverts = m[0];
        const unsigned int btris = m[2];
        const int ntris = (int)m[3];
        const float* verts = &dmesh.verts[bverts*3];
        const unsigned char* tris = &dmesh.tris[btris*4];
	
        for (int j = 0; j < ntris; ++j)
        {
            const unsigned char* t = &tris[j*4];
            for (int k = 0, kp = 2; k < 3; kp=k++)
            {
                unsigned char ef = (t[3] >> (kp*2)) & 0x3;
                if (ef == 0)
                {
                    // Internal edge
                    if (t[kp] < t[k])
                    {
                        vertices.push_back(&verts[t[kp] * 3]);
                        vertices.push_back(&verts[t[k] * 3]);
                    }
                }
            }
        }
    }
    std::vector<SyVector3> colors(vertices.size(), SyVector3::ZERO); //paint it black
    navMeshC._wasIntEdgesDataPrepared = true;
    //!!!
    //navMeshC._internalEdgesVertexBuffer->Initialize(vertices, colors);
    return SyResult();
}

SyResult SyNavMeshDrawSystem::PrepareRenderDataExternalEdges(const entt::entity& entity, SyNavMeshComponent& navMeshC)
{
    SyResult result;
    std::vector<Vector4> vertices;
    if (navMeshC.meshDetail.get() == nullptr)
    {
        SY_LOG_AI(SY_LOGLEVEL_ERROR, "No NavMesh detail found on entity (%s)", SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
        navMeshC._wasExtEdgesDataPrepared = false;
        result.code = SY_RESCODE_ERROR;
        result.message = xstring("No NavMesh detail found on entity (%s)", SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
        return result;
    }

    Vector4 tmp;
    SyVector3 tmpSy;
    const auto& dmesh = *navMeshC.meshDetail.get();
    for (int i = 0; i < dmesh.nmeshes; ++i)
    {
        const unsigned int* m = &dmesh.meshes[i*4];
        const unsigned int bverts = m[0];
        const unsigned int btris = m[2];
        const int ntris = (int)m[3];
        const float* verts = &dmesh.verts[bverts*3];
        const unsigned char* tris = &dmesh.tris[btris*4];
	
        for (int j = 0; j < ntris; ++j)
        {
            const unsigned char* t = &tris[j*4];
            for (int k = 0, kp = 2; k < 3; kp=k++)
            {
                unsigned char ef = (t[3] >> (kp*2)) & 0x3;
                if (ef != 0)
                {
                    // Ext edge
                    tmpSy = &verts[t[kp] * 3];

                    vertices.push_back(Vector4(tmpSy.x,tmpSy.y,tmpSy.z,1));
                    vertices.push_back(Vector4(0, 0, 0, 1));


                    tmpSy = &verts[t[k] * 3];
                    vertices.push_back(Vector4(tmpSy.x, tmpSy.y, tmpSy.z, 1));
                    vertices.push_back(Vector4(0, 0, 0, 1));
                }
            }
        }
    }


    std::vector<SyVector3> colors(vertices.size(), SyVector3::ZERO); //paint it black
    navMeshC._wasExtEdgesDataPrepared = false;
    //!!!
    navMeshC._externalEdgesVertexBuffer->Initialize(vertices);
    return SyResult();
}

SyVector3 SyNavMeshDrawSystem::IntToColor(int number)
{
    SyVector3 color;
    int	r = SyMathHelper::Bit(number, 0) + SyMathHelper::Bit(number, 3) * 2 + 1;
    int	g = SyMathHelper::Bit(number, 1) + SyMathHelper::Bit(number, 4) * 2 + 1;
    int	b = SyMathHelper::Bit(number, 2) + SyMathHelper::Bit(number, 5) * 2 + 1;
    color.x = 1 - r * 63.0f / 255.0f;
    color.y = 1 - g * 63.0f / 255.0f;
    color.z = 1 - b * 63.0f / 255.0f;
    return color;
}
