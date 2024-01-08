#include "NavMeshSystem.h"

#include "../../Contexts/EngineContext.h"
#include "../../Core/Tools/Macro.h"
#include "../../Mesh/Components/MeshComponent.h"
#include "../Components/NavMeshComponent.h"
//#include "Sample.h"
#include "DetourNavMeshBuilder.h"

#define LOG_AND_RETURN_ERROR(messageText, ...)  result.code = SY_RESCODE_ERROR; \
                                                result.message = xstring(messageText, __VA_ARGS__); \
                                                SY_LOG_AI(SY_LOGLEVEL_ERROR, messageText, __VA_ARGS__); \
                                                return result; \

SyResult SyNavMeshSystem::Init()
{
    SyResult result;
    result = SY_EL->RegisterChannel("AI  ");
    return SyResult();
}

SyResult SyNavMeshSystem::Run()
{
    static bool wasNavMeshBuilt = false;
    if (ServiceLocator::instance()->Get<EngineContext>()->playModeState == EngineContext::EPlayModeState::PlayMode)
        if (wasNavMeshBuilt == false)
        {
            auto view = _ecs->view<SyNavMeshComponent>();
            for (auto& entity : view)
            {
                auto prepareDataResult = PrepareData(entity);
                if (prepareDataResult.code != SY_RESCODE_OK)
                    return prepareDataResult;
                auto buildNavMeshResult = BuildNavMesh(entity);
                if (buildNavMeshResult.code != SY_RESCODE_OK)
                    return buildNavMeshResult;
            }
            wasNavMeshBuilt = true;
        }
    return SyResult();
}

SyResult SyNavMeshSystem::Destroy()
{
    auto view = _ecs->view<SyNavMeshComponent>();
    for (auto& entity : view)
    {
        auto& navMeshC = _ecs->get<SyNavMeshComponent>(entity);
        rcFreeHeightField(navMeshC.heightfield.get());
        rcFreeCompactHeightfield(navMeshC.compactHeightfield.get());
        rcFreeContourSet(navMeshC.contourSet.get());
        rcFreePolyMesh(navMeshC.mesh.get());
        rcFreePolyMeshDetail(navMeshC.meshDetail.get());
        dtFreeNavMesh(navMeshC.navMesh.get());
    }
    return SyResult();
}

SyResult SyNavMeshSystem::PrepareData(const entt::entity& entity)
{
    SyResult result;
    auto& navMeshC = _ecs->get<SyNavMeshComponent>(entity);
    auto meshView = _ecs->view<MeshComponent>();
    for (auto& meshEntity : meshView)
    {
        const auto& meshC = _ecs->get<MeshComponent>(meshEntity);
        if (meshC.flags & SyEMeshComponentFlags::AFFECTS_NAVMESH)
            for (auto& mesh : meshC.model->meshes)
            {
                navMeshC.numVerts += mesh->vertices.size();
                navMeshC.numTris += mesh->indices.size() / 3;
            }
    }
    navMeshC.vertices.resize(navMeshC.numVerts * 3);
    navMeshC.triangles.resize(navMeshC.numTris * 3);
    size_t navMeshVertexIndex = 0;
    size_t navMeshTriangleIndex = 0;
    for (auto& meshEntity : meshView)
    {
        const auto& meshC = _ecs->get<MeshComponent>(meshEntity);
        const auto& transformC = _ecs->get<TransformComponent>(meshEntity);
        if (meshC.flags & SyEMeshComponentFlags::AFFECTS_NAVMESH)
            for (auto& mesh : meshC.model->meshes)
            {
                for (unsigned i = 0; i < mesh->vertices.size(); i++)
                {
                    auto transformedVertex = DirectX::SimpleMath::Vector4::Transform(mesh->vertices[i], transformC.transformMatrix);
                    navMeshC.vertices[(navMeshVertexIndex + i) * 3    ] = transformedVertex.x;
                    navMeshC.vertices[(navMeshVertexIndex + i) * 3 + 1] = transformedVertex.y;
                    navMeshC.vertices[(navMeshVertexIndex + i) * 3 + 2] = transformedVertex.z;
                }
                for (unsigned i = 0; i < mesh->indices.size(); i++)
                    navMeshC.triangles[navMeshTriangleIndex + i] = mesh->indices[i] + navMeshVertexIndex;
                navMeshVertexIndex += mesh->vertices.size();
                navMeshTriangleIndex += mesh->indices.size();
            }
    }
    return SyResult();
}

SyResult SyNavMeshSystem::BuildNavMesh(const entt::entity& entity)
{
    SyResult result;
    auto* entityName = SY_GET_ENTITY_NAME_CHAR(_ecs, entity);
    auto& navMeshC = _ecs->get<SyNavMeshComponent>(entity);

#pragma region 1. Initialize build config
    memset(&navMeshC.buildConfig, 0, sizeof(navMeshC.buildConfig));
    auto bmin = navMeshC.origin - navMeshC.extent;
    navMeshC.buildConfig.bmin[0] = bmin.x;
    navMeshC.buildConfig.bmin[1] = bmin.y;
    navMeshC.buildConfig.bmin[2] = bmin.z;
    auto bmax = navMeshC.origin + navMeshC.extent;
    navMeshC.buildConfig.bmax[0] = bmax.x;
    navMeshC.buildConfig.bmax[1] = bmax.y;
    navMeshC.buildConfig.bmax[2] = bmax.z;
    navMeshC.buildConfig.cs = navMeshC.cellSize;
    navMeshC.buildConfig.ch = navMeshC.cellHeight;
    navMeshC.buildConfig.walkableSlopeAngle = navMeshC.agentMaxWalkableSlopeAngle;
    navMeshC.buildConfig.walkableHeight = (int)ceilf(navMeshC.agentHeight / navMeshC.buildConfig.ch);
    navMeshC.buildConfig.walkableClimb = (int)floorf(navMeshC.agentMaxClimb / navMeshC.buildConfig.ch);
    navMeshC.buildConfig.walkableRadius = (int)ceilf(navMeshC.agentRadius / navMeshC.buildConfig.cs);
    navMeshC.buildConfig.maxEdgeLen = (int)(navMeshC.edgeMaxLen / navMeshC.cellSize);
    navMeshC.buildConfig.maxSimplificationError = navMeshC.edgeMaxError;
    navMeshC.buildConfig.minRegionArea = (int)rcSqr(navMeshC.regionMinSize);		// Note: area = size*size
    navMeshC.buildConfig.mergeRegionArea = (int)rcSqr(navMeshC.regionMergeSize);	// Note: area = size*size
    navMeshC.buildConfig.maxVertsPerPoly = navMeshC.maxVertsPerPoly;
    navMeshC.buildConfig.detailSampleDist = navMeshC.detailSampleDist < 0.9f ? 0 : navMeshC.cellSize * navMeshC.detailSampleDist;
    navMeshC.buildConfig.detailSampleMaxError = navMeshC.cellHeight * navMeshC.detailSampleMaxError;
    rcCalcGridSize( navMeshC.buildConfig.bmin, navMeshC.buildConfig.bmax, navMeshC.buildConfig.cs,
                    &navMeshC.buildConfig.width, &navMeshC.buildConfig.height);
#pragma endregion
#pragma region 2. Rasterize input polygon soup
    navMeshC.recastContext->startTimer(RC_TIMER_TOTAL);
    navMeshC.heightfield = std::shared_ptr<rcHeightfield>(rcAllocHeightfield());
    if (!navMeshC.heightfield)
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
        LOG_AND_RETURN_ERROR("Could not allocate memory for heightfield for NavMesh Component on entity (%s). ", entityName);
    }
    if (!rcCreateHeightfield(navMeshC.recastContext.get(), *navMeshC.heightfield,
        navMeshC.buildConfig.width, navMeshC.buildConfig.height,
        navMeshC.buildConfig.bmin, navMeshC.buildConfig.bmax,
        navMeshC.buildConfig.cs, navMeshC.buildConfig.ch))
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
        LOG_AND_RETURN_ERROR("Could not create heightfield for NavMesh Component on entity (%s). ", entityName);
    }

    // Allocate array that can hold triangle area types.
    // If you have multiple meshes you need to process, allocate
    // and array which can hold the max number of triangles you need to process.
    navMeshC.triAreaTypes = std::vector<unsigned char>(navMeshC.numTris, 0);


    // Find triangles which are walkable based on their slope and rasterize them.
    // If your input data is multiple meshes, you can transform them here, calculate
    // the are type for each of the meshes and rasterize them.
    //memset(m_triareas, 0, ntris*sizeof(unsigned char));
    rcMarkWalkableTriangles(navMeshC.recastContext.get(),
                            navMeshC.buildConfig.walkableSlopeAngle,
                            navMeshC.vertices.begin()._Ptr,
                            navMeshC.numVerts,
                            navMeshC.triangles.begin()._Ptr,
                            navMeshC.numTris,
                            navMeshC.triAreaTypes.begin()._Ptr,
                            navMeshC.SAMPLE_AREAMOD_GROUND);
    if (!rcRasterizeTriangles(  navMeshC.recastContext.get(),
                                navMeshC.vertices.begin()._Ptr,
                                navMeshC.numVerts,
                                navMeshC.triangles.begin()._Ptr,
                                navMeshC.triAreaTypes.begin()._Ptr,
                                navMeshC.numTris,
                                *navMeshC.heightfield,
                                navMeshC.buildConfig.walkableClimb))
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not rasterize triangles.");
        LOG_AND_RETURN_ERROR("Could not rasterize triangles for NavMesh Component on entity (%s). ", entityName);
    }

    // if (!m_keepInterResults)
    // {
    //     delete [] m_triareas;
    //     m_triareas = 0;
    // }
#pragma endregion
#pragma region 3. Filter walkables surfaces
    rcFilterLowHangingWalkableObstacles(navMeshC.recastContext.get(),
                                        navMeshC.buildConfig.walkableClimb,
                                        *navMeshC.heightfield);
    rcFilterLedgeSpans( navMeshC.recastContext.get(),
                        navMeshC.buildConfig.walkableHeight,
                        navMeshC.buildConfig.walkableClimb,
                        *navMeshC.heightfield);
    rcFilterWalkableLowHeightSpans( navMeshC.recastContext.get(),
                                    navMeshC.buildConfig.walkableHeight,
                                    *navMeshC.heightfield);
#pragma endregion
#pragma region 4. Partition walkable surface to simple regions
    navMeshC.compactHeightfield = std::shared_ptr<rcCompactHeightfield>(rcAllocCompactHeightfield());
    if (!navMeshC.compactHeightfield)
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
        LOG_AND_RETURN_ERROR("Could not allocate memory for compact heightfield for NavMesh Component on entity (%s). ", entityName);
    }
    if (!rcBuildCompactHeightfield( navMeshC.recastContext.get(), navMeshC.buildConfig.walkableHeight,
                                    navMeshC.buildConfig.walkableClimb,
                                    *navMeshC.heightfield, *navMeshC.compactHeightfield))
    {
         navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
        LOG_AND_RETURN_ERROR("Could not build compact heightfield for NavMesh Component on entity (%s). ", entityName);
    }
    if (!rcErodeWalkableArea(   navMeshC.recastContext.get(),
                                navMeshC.buildConfig.walkableRadius, *navMeshC.compactHeightfield.get()))
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
        LOG_AND_RETURN_ERROR("Could not erode for NavMesh Component on entity (%s). ", entityName);
    }
    
    if (navMeshC.partitioningType == Watershed)
    {
        if (!rcBuildDistanceField(navMeshC.recastContext.get(), *navMeshC.compactHeightfield))
        {
            navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
            LOG_AND_RETURN_ERROR("Could not build distance field for NavMesh Component on entity (%s). ", entityName);
        }
        if (!rcBuildRegions(navMeshC.recastContext.get(), *navMeshC.compactHeightfield,
                            0, navMeshC.buildConfig.minRegionArea, navMeshC.buildConfig.mergeRegionArea))
        {
            navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
            LOG_AND_RETURN_ERROR("Could not build watershed regions for NavMesh Component on entity (%s). ", entityName);
        }
    }
    else if (navMeshC.partitioningType == Monotone)
    {
        if (!rcBuildRegionsMonotone(navMeshC.recastContext.get(), *navMeshC.compactHeightfield,
                                    0, navMeshC.buildConfig.minRegionArea, navMeshC.buildConfig.mergeRegionArea))
        {
            navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
            LOG_AND_RETURN_ERROR("Could not build monotone regions for NavMesh Component on entity (%s). ", entityName);
        }
    }
    else
        {
        if (!rcBuildLayerRegions(navMeshC.recastContext.get(), *navMeshC.compactHeightfield,
                                 0, navMeshC.buildConfig.minRegionArea))
        {
            navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
            LOG_AND_RETURN_ERROR("Could not build layer regions for NavMesh Component on entity (%s). ", entityName);
        }
        }
#pragma endregion
#pragma region 5. Trace and simplify region contours
    navMeshC.contourSet = std::shared_ptr<rcContourSet>(rcAllocContourSet());
    if (!navMeshC.contourSet)
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
        LOG_AND_RETURN_ERROR("Could not allocate memory for contour set for NavMesh Component on entity (%s). ", entityName)
    }
    if (!rcBuildContours(   navMeshC.recastContext.get(), *navMeshC.compactHeightfield,
                            navMeshC.buildConfig.maxSimplificationError,
                            navMeshC.buildConfig.maxEdgeLen, *navMeshC.contourSet))
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
        LOG_AND_RETURN_ERROR("Could not build contours for NavMesh Component on entity (%s). ", entityName);
    }
#pragma endregion
#pragma region 6. Build polygons mesh from contours
    navMeshC.mesh = std::shared_ptr<rcPolyMesh>(rcAllocPolyMesh());
    if (!navMeshC.mesh)
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
        LOG_AND_RETURN_ERROR("Could not allocate memory for mesh for NavMesh Component on entity (%s). ", entityName)
    }
    if (!rcBuildPolyMesh(navMeshC.recastContext.get(), *navMeshC.contourSet, navMeshC.buildConfig.maxVertsPerPoly, *navMeshC.mesh))
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
        LOG_AND_RETURN_ERROR("Could not build mesh for NavMesh Component on entity (%s). ", entityName);
    }
#pragma endregion
#pragma region 7. Create detail mesh which allows to access approximate height on each polygon
    navMeshC.meshDetail = std::shared_ptr<rcPolyMeshDetail>(rcAllocPolyMeshDetail());
    if (!navMeshC.meshDetail)
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
        LOG_AND_RETURN_ERROR("Could not allocate memory for detail mesh for NavMesh Component on entity (%s). ", entityName)
    }
    if (!rcBuildPolyMeshDetail(navMeshC.recastContext.get(),
                *navMeshC.mesh,
                *navMeshC.compactHeightfield,
                navMeshC.buildConfig.detailSampleDist,
                navMeshC.buildConfig.detailSampleMaxError,
                *navMeshC.meshDetail))
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
        LOG_AND_RETURN_ERROR("Could not build detail mesh for NavMesh Component on entity (%s). ", entityName);
    }
#pragma endregion
#pragma region 8. Create Detour data from Recast poly mesh
    assert(navMeshC.maxVertsPerPoly <= SyNavMeshComponent::MAX_VERTS_PER_POLYGON);
    unsigned char* navData = 0;
    int navDataSize = 0;

    // Update poly flags from areas.
    // for (int i = 0; i < navMeshC.mesh->npolys; ++i)
    // {
	   //  navMeshC.mesh->flags[i] = sampleAreaToFlags(navMeshC.mesh->areas[i]);
    // }


    dtNavMeshCreateParams params;
    memset(&params, 0, sizeof(params));
    params.verts = navMeshC.mesh->verts;
    params.vertCount = navMeshC.mesh->nverts;
    params.polys = navMeshC.mesh->polys;
    params.polyAreas = navMeshC.mesh->areas;
    params.polyFlags = navMeshC.mesh->flags;
    params.polyCount = navMeshC.mesh->npolys;
    params.nvp = navMeshC.mesh->nvp;
    params.detailMeshes = navMeshC.meshDetail->meshes;
    params.detailVerts = navMeshC.meshDetail->verts;
    params.detailVertsCount = navMeshC.meshDetail->nverts;
    params.detailTris = navMeshC.meshDetail->tris;
    params.detailTriCount = navMeshC.meshDetail->ntris;
    params.offMeshConVerts = nullptr;
    params.offMeshConRad = nullptr;
    params.offMeshConDir = nullptr;
    params.offMeshConAreas = nullptr;
    params.offMeshConFlags = nullptr;
    params.offMeshConUserID = nullptr;
    params.offMeshConCount = 0;
    params.walkableHeight = navMeshC.agentHeight;
    params.walkableRadius = navMeshC.agentRadius;
    params.walkableClimb = navMeshC.agentMaxClimb;
    rcVcopy(params.bmin, navMeshC.mesh->bmin);
    rcVcopy(params.bmax, navMeshC.mesh->bmax);
    params.cs = navMeshC.buildConfig.cs;
    params.ch = navMeshC.buildConfig.ch;
    params.buildBvTree = true;

    if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "Could not create navmesh data .");
	    LOG_AND_RETURN_ERROR("Could not create navmesh data for NavMesh component for entity (%s). ", entityName)
    }

    navMeshC.navMesh = std::shared_ptr<dtNavMesh>(dtAllocNavMesh());
    if (!navMeshC.navMesh)
    {
	    dtFree(navData);
	    navMeshC.recastContext->log(RC_LOG_ERROR, "Could not create Detour navmesh");
        LOG_AND_RETURN_ERROR("Could not create Detour navmesh for NavMesh component for entity (%s). ", entityName)
    }
    dtStatus status;
    status = navMeshC.navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
    if (dtStatusFailed(status))
    {
	    dtFree(navData);
	    navMeshC.recastContext->log(RC_LOG_ERROR, "Could not init Detour navmesh");
        LOG_AND_RETURN_ERROR("Could not init Detour navmesh for NavMesh component for entity (%s). ", entityName)
    }

    status = navMeshC.navQuery->init(navMeshC.navMesh.get(), 2048);
    if (dtStatusFailed(status))
    {
	    navMeshC.recastContext->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
        LOG_AND_RETURN_ERROR("Could not init Detour navmesh query for NavMesh component for entity (%s). ", entityName)
    }
#pragma endregion
    navMeshC.recastContext->stopTimer(RC_TIMER_TOTAL);
    
    SY_LOG_AI(SY_LOGLEVEL_INFO,     "NavMesh build complete. Polymesh: %d vertices  %d polygons",
                                    navMeshC.mesh->nverts,
                                    navMeshC.mesh->npolys);
    return SyResult();
}
