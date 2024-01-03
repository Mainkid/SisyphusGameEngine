#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Features/Mesh/Components/MeshComponent.h"

namespace mono
{
	struct ProxyMeshComp
	{
		MonoString* ModelUuid;
		MonoArray* MaterialsUuids;
	};


	class SyMonoEcsSyncMesh :
		public SyMonoEcsSyncBase<SyMonoEcsSyncMesh, MeshComponent, ProxyMeshComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::Mesh; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateMeshComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateMeshComp"; }

		void FillProxyByComp(const MeshComponent& comp) override;
		void FillCompByProxy(const ProxyMeshComp& proxy, entt::entity ent, MeshComponent& comp) override;
		size_t GetHash(const MeshComponent& comp) override;
	};
}
