#pragma once
#include "SyMonoEcsSyncBase.h"
#include "SyMonoProxyDatas.h"
#include "../../Features/AI/Components/NavMeshComponent.h"

namespace mono
{
    struct ProxyNavMeshComp
    {
        ProxyVector3 Origin;
        ProxyVector3 Extent;
        float CellSize;
        float CellHeight;
        float AgentMaxWalkableSlopeAngle;
        float AgentHeight;
        float AgentMaxClimb;
        float AgentRadius;
        float EdgeMaxError;
        float EdgeMaxLen;
        float RegionMinSize;
        float RegionMergeSize;
        int MaxVertsPerPoly;
        float DetailSampleDist;
        float DetailSampleMaxError;

        ESyNavMeshPartitionType PartitioningType;
    };

	class SyMonoEcsSyncNavMesh : public SyMonoEcsSyncBase<SyNavMeshComponent, ProxyNavMeshComp>
	{
	public:
        ECompId GetCompId() const override { return ECompId::NavMesh; }
		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
        std::string GetEgMethodName() const override { return "EgUpdateNavMeshComp"; }
        std::string GetGeMethodName() const override { return "GeUpdateNavMeshComp"; }
		void FillProxyByComp(const SyNavMeshComponent& comp) override;
		void FillCompByProxy(const ProxyNavMeshComp& proxy, entt::entity ent, SyNavMeshComponent& comp) override;
		size_t GetHash(const SyNavMeshComponent& comp) override;
	};
}
