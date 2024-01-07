#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Features/Animations/Components/AnimatorComponent.h"

namespace mono
{
	struct ProxyAnimatorComp
	{
		MonoString* AnimationUuid;
		bool IsLooping;
	};

	class SyMonoEcsSyncAnimator : public SyMonoEcsSyncBase<AnimatorComponent, ProxyAnimatorComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::Animator; }
		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateAnimatorComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateAnimatorComp"; }

		void FillProxyByComp(const AnimatorComponent& comp) override;
		void FillCompByProxy(const ProxyAnimatorComp& proxy, entt::entity ent, AnimatorComponent& comp) override;
		size_t GetHash(const AnimatorComponent& comp) override;
	};
}
