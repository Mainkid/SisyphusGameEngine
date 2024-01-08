#include "SyMonoEcsSyncNavMesh.h"

#include "../../Features/Common/Events/CompAddedEv.h"
#include "../../Features/Common/Events/CompRemovedEv.h"
#include "../../Scene/GameObjectHelper.h"

void mono::SyMonoEcsSyncNavMesh::AddComp(entt::entity ent)
{
	_ecs->emplace<SyNavMeshComponent>(ent);
	GameObjectHelper::CallEvent<CompAddedEv>(_ecs, "Mono", GetCompId(), ent);
}

void mono::SyMonoEcsSyncNavMesh::RemoveComp(entt::entity ent)
{
	_ecs->remove<SyNavMeshComponent>(ent);
	GameObjectHelper::CallEvent<CompRemovedEv>(_ecs, "Mono", GetCompId(), ent);
}

void mono::SyMonoEcsSyncNavMesh::FillProxyByComp(const SyNavMeshComponent& comp)
{
    _proxy.Origin = comp.origin;
    _proxy.Extent = comp.extent;
    _proxy.CellSize = comp.cellSize;
    _proxy.CellHeight = comp.cellHeight;
    _proxy.AgentMaxWalkableSlopeAngle = comp.agentMaxWalkableSlopeAngle;
    _proxy.AgentHeight = comp.agentHeight;
    _proxy.AgentMaxClimb = comp.agentMaxClimb;
    _proxy.AgentRadius = comp.agentRadius;
    _proxy.EdgeMaxError = comp.edgeMaxError;
    _proxy.EdgeMaxLen = comp.edgeMaxLen;
    _proxy.RegionMinSize = comp.regionMinSize;
    _proxy.RegionMergeSize = comp.regionMergeSize;
    _proxy.MaxVertsPerPoly = comp.maxVertsPerPoly;
    _proxy.DetailSampleDist = comp.detailSampleDist;
    _proxy.DetailSampleMaxError = comp.detailSampleMaxError;
	_proxy.PartitioningType = comp.partitioningType;
}

void mono::SyMonoEcsSyncNavMesh::FillCompByProxy(const ProxyNavMeshComp& proxy, entt::entity ent,
	SyNavMeshComponent& comp)
{
    comp.origin = proxy.Origin;
    comp.extent = proxy.Extent;
    comp.cellSize = proxy.CellSize;
    comp.cellHeight = proxy.CellHeight;
    comp.agentMaxWalkableSlopeAngle = proxy.AgentMaxWalkableSlopeAngle;
    comp.agentHeight = proxy.AgentHeight;
    comp.agentMaxClimb = proxy.AgentMaxClimb;
    comp.agentRadius = proxy.AgentRadius;
    comp.edgeMaxError = proxy.EdgeMaxError;
    comp.edgeMaxLen = proxy.EdgeMaxLen;
    comp.regionMinSize = proxy.RegionMinSize;
    comp.regionMergeSize = proxy.RegionMergeSize;
    comp.maxVertsPerPoly = proxy.MaxVertsPerPoly;
    comp.detailSampleDist = proxy.DetailSampleDist;
    comp.detailSampleMaxError = proxy.DetailSampleMaxError;
    comp.partitioningType = proxy.PartitioningType;

    comp.IsMonoDirty = false;
}

size_t mono::SyMonoEcsSyncNavMesh::GetHash(const SyNavMeshComponent& comp)
{
    return comp.IsMonoDirty ? 1 : 0;
}
