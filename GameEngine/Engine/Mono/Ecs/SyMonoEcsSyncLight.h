#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Features/Lighting/Components/LightComponent.h"

namespace mono
{
	struct ProxyLightComp
	{
		ELightType LightType;
		LightBehavior LightBehavior;
		ProxyVector4 Color;
		float PointLightRadius;
		bool ShouldCastShadows;
	};


	class SyMonoEcsSyncLight :
		public SyMonoEcsSyncBase<SyMonoEcsSyncLight, LightComponent, ProxyLightComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::Light; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateLightComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateLightComp"; }

		void FillProxyByComp(const LightComponent& comp) override;
		void FillCompByProxy(const ProxyLightComp& proxy, entt::entity ent, LightComponent& comp) override;
		size_t GetHash(const LightComponent& comp) override;
	};
}
