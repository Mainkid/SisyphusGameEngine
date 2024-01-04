#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Components/SkyboxComponent.h"

namespace mono
{
	struct ProxySkyboxComp
	{
		MonoString* CubemapUuid;
	};


	class SyMonoEcsSyncSkybox : public SyMonoEcsSyncBase<SkyboxComponent, ProxySkyboxComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::Skybox; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateSkyboxComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateSkyboxComp"; }

		void FillProxyByComp(const SkyboxComponent& comp) override;
		void FillCompByProxy(const ProxySkyboxComp& proxy, entt::entity ent, SkyboxComponent& comp) override;
		size_t GetHash(const SkyboxComponent& comp) override;
	};
}
