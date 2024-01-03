#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Engine/Components/TransformComponent.h"

namespace mono
{
	struct ProxyTransformComp
	{
		ProxyVector3 Position;
		ProxyVector3 Rotation;
		ProxyVector3 Scale;
		ProxyVector3 LocalPosition;
		ProxyVector3 LocalRotation;
		ProxyVector3 LocalScale;

		bool HasParent;
		uint32_t ParentEngineEnt;
	};


	class SyMonoEcsSyncTransform :
		public SyMonoEcsSyncBase<SyMonoEcsSyncTransform, TransformComponent, ProxyTransformComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::Transform; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateTransformComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateTransformComp"; }

		void FillProxyByComp(const TransformComponent& comp) override;
		void FillCompByProxy(const ProxyTransformComp& proxy, entt::entity ent, TransformComponent& comp) override;
		size_t GetHash(const TransformComponent& comp) override;
	};
}
