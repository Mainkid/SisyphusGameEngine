#pragma once

#include "SyMonoEcsSyncBase.h"
#include "../../Features/Physics/Components/JointComponent.h"

namespace mono
{
	struct ProxyFixedJointComp
	{
		bool IsAttachedToEnt;
		uint32_t AttachedToEnt;
	};

	class SyMonoEcsSyncFixedJoint : public SyMonoEcsSyncBase<SyFixedJointComponent, ProxyFixedJointComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::FixedJoint; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateFixedJointComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateFixedJointComp"; }

		void FillProxyByComp(const SyFixedJointComponent& comp) override;
		void FillCompByProxy(const ProxyFixedJointComp& proxy, entt::entity ent, SyFixedJointComponent& comp) override;
		size_t GetHash(const SyFixedJointComponent& comp) override;
	};
}
