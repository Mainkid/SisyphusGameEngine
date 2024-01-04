#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Components/CameraComponent.h"

namespace mono
{
	struct ProxyCameraComp
	{
		float NearPlane;
		float FarPlane;
		float FovDegrees;
	};

	class SyMonoEcsSyncCamera : public SyMonoEcsSyncBase<CameraComponent, ProxyCameraComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::Camera; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateCameraComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateCameraComp"; }

		void FillProxyByComp(const CameraComponent& comp) override;
		void FillCompByProxy(const ProxyCameraComp& proxy, entt::entity ent, CameraComponent& comp) override;
		size_t GetHash(const CameraComponent& comp) override;
	};
}
