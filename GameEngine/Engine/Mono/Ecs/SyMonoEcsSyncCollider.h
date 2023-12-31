#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Features/Physics/Components/ColliderComponent.h"

namespace mono
{
	struct ProxyColliderComp
	{
		SyEPrimitiveColliderType Type;
		ProxyVector3 Extent;
		float Radius;
		float HalfHeight;
	};

	class SyMonoEcsSyncCollider :
		public SyMonoEcsSyncBase<SyMonoEcsSyncCollider, SyPrimitiveColliderComponent, ProxyColliderComp>
	{
	public:
		EProxyCompId GetCompId() const override { return EProxyCompId::Collider; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateColliderComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateColliderComp"; }

		void FillProxyByComp(const SyPrimitiveColliderComponent& comp) override;
		void FillCompByProxy(const ProxyColliderComp& proxy, entt::entity ent,
			SyPrimitiveColliderComponent& comp) override;
		size_t GetHash(const SyPrimitiveColliderComponent& comp) override;
	};
}
