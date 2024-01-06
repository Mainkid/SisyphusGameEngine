#include "NavMeshDrawSystem.h"


#include "../../../Contexts/EngineContext.h"
#include "../../../Core/Tools/Macro.h"
#include "../Components/NavMeshComponent.h"
#include "../../../Core/Tools/Structures/Geometry.h"
#define CHECK_RESULT(messageText, ...)    if (result.code == SY_RESCODE_ERROR) \
                                                       {\
                                                            SY_LOG_AI(SY_LOGLEVEL_ERROR, messageText, __VA_ARGS__); \
                                                            continue; \
                                                       } \
                                                    
SyResult SyNavMeshDrawSystem::Init()
{
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
            auto* entityName = SY_GET_ENTITY_NAME_CHAR(_ecs, entity);
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
    return SyResult();
}

SyResult SyNavMeshDrawSystem::Destroy()
{
    return SyResult();
}

SyResult SyNavMeshDrawSystem::PrepareRenderDataAabb(const entt::entity& entity, SyNavMeshComponent& navMeshC)
{
    std::vector<SyVector3> vertices(8);
    auto e = navMeshC.extent;
    auto o = navMeshC.origin;
    vertices[0] = o + e.ReflectX().ReflectZ();
    vertices[1] = o + e.ReflectX();
    vertices[2] = o + e;
    vertices[3] = o + e.ReflectZ();
    vertices[4] = o + e.ReflectX().ReflectZ().ReflectY();
    vertices[5] = o + e.ReflectX().ReflectY();
    vertices[6] = o + e.ReflectY();
    vertices[7] = o + e.ReflectZ().ReflectY();
    std::vector<int> indices = {0,1,2,3,0, 0,3,7,4,0, 0,1,5,4,0, 1,2,6,5, 3,2,6,7,3, 4,5,6,7,4};
    navMeshC._aabbVertexBuffer->Initialize(vertices);
    navMeshC._aabbIndexBuffer->Initialize(indices);
    return SyResult();    
}

SyResult SyNavMeshDrawSystem::PrepareRenderDataMesh(const entt::entity& entity, SyNavMeshComponent& navMeshC)
{
    SyResult result;
    std::vector<SyVector3> vertices;
    std::vector<SyVector3> colors;
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
        SyVector3 color = IntToColor(i);
        for (int j = 0; j < ntris; ++j)
        {
            vertices.push_back(&verts[tris[j * 4 + 0] * 3]);
            vertices.push_back(&verts[tris[j * 4 + 1] * 3]);
            vertices.push_back(&verts[tris[j * 4 + 2] * 3]);
            colors.insert(colors.end(), 3, color);
        }
    }
    //!!!
    //navMeshC._meshVertexBuffer->Initialize(vertices, colors)
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
    std::vector<SyVector3> vertices;
    if (navMeshC.meshDetail.get() == nullptr)
    {
        SY_LOG_AI(SY_LOGLEVEL_ERROR, "No NavMesh detail found on entity (%s)", SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
        navMeshC._wasExtEdgesDataPrepared = false;
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
                if (ef != 0)
                {
                    // Ext edge
                    vertices.push_back(&verts[t[kp] * 3]);
                    vertices.push_back(&verts[t[k] * 3]);
                }
            }
        }
    }
    std::vector<SyVector3> colors(vertices.size(), SyVector3::ZERO); //paint it black
    navMeshC._wasExtEdgesDataPrepared = false;
    //!!!
    //navMeshC._externalEdgesVertexBuffer->Initialize(vertices, colors);
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
