#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Features/Physics/Components/RBodyComponent.h"

namespace mono
{
	struct ProxyRigidComp
	{
		SyERBodyType Type;
		float Mass;

		bool IsAutoMass;
		bool IsKinematic;
		bool IsGravityOn;

		ProxyVector3 LinearVelocity;
		ProxyVector3 AngularVelocity;
	};


	class SyMonoEcsSyncRigid :
		public SyMonoEcsSyncBase<SyMonoEcsSyncRigid, SyRigidBodyComponent, ProxyRigidComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::Rigid; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateRigidComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateRigidComp"; }

		void FillProxyByComp(const SyRigidBodyComponent& comp) override;
		void FillCompByProxy(const ProxyRigidComp& proxy, entt::entity ent, SyRigidBodyComponent& comp) override;
		size_t GetHash(const SyRigidBodyComponent& comp) override;
	};
}
