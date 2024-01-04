#include "NavMeshSystem.h"
#include "../../Contexts/EngineContext.h"
#include "../../Core/Tools/Macro.h"
#include "../Components/NavMeshComponent.h"


#define LOG_AND_RETURN_ERROR(messageText, ...)  result.code = SY_RESCODE_ERROR; \
                                                result.message = xstring(messageText, __VA_ARGS__); \
                                                SY_LOG_AI(SY_LOGLEVEL_ERROR, messageText, __VA_ARGS__); \
                                                return result; \

SyResult SyNavMeshSystem::Init()
{
    SY_EL->RegisterChannel("AI  ");
    return SyResult();
}

SyResult SyNavMeshSystem::Run()
{
    return SyResult();
}

SyResult SyNavMeshSystem::Destroy()
{
    return SyResult();
}

SyResult SyNavMeshSystem::BuildNavMesh(const entt::entity& entity)
{
    SyResult result;
    auto* entityName = SY_GET_ENTITY_NAME_CHAR(_ecs, entity);
    auto& navMeshC = _ecs->get<SyNavMeshComponent>(entity);

    if (navMeshC.partitioningType == Watershed)
    {
        // Prepare for region partitioning, by calculating distance field along the walkable surface.
        if (!rcBuildDistanceField(navMeshC.recastContext, *navMeshC.compactHeightfield))
        {
            navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
            return false;
        }
		
        // Partition the walkable surface into simple regions without holes.
        if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
            return false;
        }
    }
    else if (m_partitionType == SAMPLE_PARTITION_MONOTONE)
    {
        // Partition the walkable surface into simple regions without holes.
        // Monotone partitioning does not need distancefield.
        if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
            return false;
        }
    }
    else // SAMPLE_PARTITION_LAYERS
        {
        // Partition the walkable surface into simple regions without holes.
        if (!rcBuildLayerRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
            return false;
        }
        }
    navMeshC.contourSet = std::shared_ptr<rcContourSet>(rcAllocContourSet());
    if (!navMeshC.contourSet)
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
        LOG_AND_RETURN_ERROR("Could not allocate memory for contour set for NavMesh Component on entity (%s). ", entityName)
    }
    if (!rcBuildContours(   navMeshC.recastContext, *navMeshC.compactHeightfield,
                            navMeshC.buildConfig.maxSimplificationError,
                            navMeshC.buildConfig.maxEdgeLen, *navMeshC.contourSet))
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
        LOG_AND_RETURN_ERROR("Could not build contours for NavMesh Component on entity (%s). ", entityName);
    }
    navMeshC.mesh = std::shared_ptr<rcPolyMesh>(rcAllocPolyMesh());
    if (!navMeshC.mesh)
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
        LOG_AND_RETURN_ERROR("Could not allocate memory for mesh for NavMesh Component on entity (%s). ", entityName)
    }
    if (!rcBuildPolyMesh(navMeshC.recastContext, *navMeshC.contourSet, navMeshC.buildConfig.maxVertsPerPoly, *navMeshC.mesh))
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
        LOG_AND_RETURN_ERROR("Could not build mesh for NavMesh Component on entity (%s). ", entityName);
    }
    navMeshC.meshDetail = std::shared_ptr<rcPolyMeshDetail>(rcAllocPolyMeshDetail());
    if (!navMeshC.meshDetail)
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
        LOG_AND_RETURN_ERROR("Could not allocate memory for detail mesh for NavMesh Component on entity (%s). ", entityName)
    }
    if (!rcBuildPolyMeshDetail(navMeshC.recastContext,
                *navMeshC.mesh,
                *navMeshC.compactHeightfield,
                navMeshC.buildConfig.detailSampleDist,
                navMeshC.buildConfig.detailSampleMaxError,
                *navMeshC.meshDetail))
    {
        navMeshC.recastContext->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
        LOG_AND_RETURN_ERROR("Could not build detail mesh for NavMesh Component on entity (%s). ", entityName);
    }
}
