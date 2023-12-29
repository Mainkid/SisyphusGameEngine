#pragma once

#include "../../Core/ECS/SystemBase.h"
#include "../Objects/ISyMonoEcsCallbacks.h"

struct EngineContext;

namespace mono
{
	class SyMonoEcs;
	class SyMonoGame;
}

class MonoSyncGeSystem : public SystemBase, mono::ISyMonoEcsCallbacks
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

private:
	mono::SyMonoEcs* _monoEcs = nullptr;
	mono::SyMonoGame* _monoGame = nullptr;

	std::chrono::time_point<std::chrono::steady_clock> _testTimeOnPrevFrame;
	float _testTotalTime = 0;


	uint32_t OnCreateEntity() override;
	void OnDestroyEntity(uint32_t rawEnt) override;
	void OnDestroyEngineEntityImpl(entt::entity ent, bool isRecursionStep);

	void OnAddComp(uint32_t rawEnt, mono::EProxyCompId id) override;
	void OnRemoveComp(uint32_t rawEnt, mono::EProxyCompId id) override;

	void OnUpdateSceneObjectComp(uint32_t rawEnt, const mono::ProxySceneObjectComp& proxy) override;
	void OnUpdateTransformComp(uint32_t rawEnt, const mono::ProxyTransformComp& proxy) override;
	void OnUpdateMeshComp(uint32_t rawEnt, const mono::ProxyMeshComp& proxy) override;
	void OnUpdateLightComp(uint32_t rawEnt, const mono::ProxyLightComp& proxy) override;
	void OnUpdateColliderComp(uint32_t rawEnt, const mono::ProxyColliderComp& proxy) override;
	void OnUpdateRigidComp(uint32_t rawEnt, const mono::ProxyRigidComp& proxy) override;
	void OnUpdateSkyboxComp(uint32_t rawEnt, const mono::ProxySkyboxComp& proxy) override;
	void OnUpdateParticlesComp(uint32_t rawEnt, const mono::ProxyParticlesComp& proxy) override;
};
