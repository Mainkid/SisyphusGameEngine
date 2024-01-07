#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Features/Physics/Components/JointComponent.h"

namespace mono
{
	struct ProxyHingeJointComp
	{
		bool IsAttachedToEnt;
		uint32_t AttachedToEngineEnt;

		ProxyVector3 PivotPos;
		ProxyVector3 PivotRot;
	};

	class SyMonoEcsSyncHingeJoint : public SyMonoEcsSyncBase<SyHingeJointComponent, ProxyHingeJointComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::HingeJoint; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateHingeJointComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateHingeJointComp"; }

		void FillProxyByComp(const SyHingeJointComponent& comp) override;
		void FillCompByProxy(const ProxyHingeJointComp& proxy, entt::entity ent, SyHingeJointComponent& comp) override;
		size_t GetHash(const SyHingeJointComponent& comp) override;
	};
}
