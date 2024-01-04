#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Components/GameObjectComp.h"

namespace mono
{
	struct ProxySceneObjectComp
	{
		MonoString* Name;
		bool IsActive;
	};


	class SyMonoEcsSyncSceneObject : public SyMonoEcsSyncBase<GameObjectComp, ProxySceneObjectComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::SceneObject; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateSceneObjectComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateSceneObjectComp"; }

		void FillProxyByComp(const GameObjectComp& comp) override;
		void FillCompByProxy(const ProxySceneObjectComp& proxy, entt::entity ent, GameObjectComp& comp) override;

		size_t GetHash(const GameObjectComp& comp) override;
	};
}