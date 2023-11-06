#pragma once

#include "../../Core/ECS/SystemBase.h"
#include "../ISyMonoGameCallbackReceiver.h"

struct EngineContext;

namespace mono
{
	class SyMonoEcs;
	class SyMonoGame;
}

class MonoSyncSystem : public SystemBase, mono::ISyMonoGameCallbackReceiver
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

private:
	mono::SyMonoEcs* _monoEcs = nullptr;
	mono::SyMonoGame* _monoGame = nullptr;

	bool _isGameInited = false;

	std::chrono::time_point<std::chrono::steady_clock> _testTimeOnPrevFrame;
	float _testTotalTime = 0;


	uint32_t OnCreateEngineEntity() override;
	void OnDestroyEngineEntity(uint32_t rawEnt) override;
	void OnDestroyEngineEntityImpl(entt::entity ent, bool isRecursionStep);

	void OnAddTransformComp(uint32_t rawEnt) override;
	void OnRemoveTransformComp(uint32_t rawEnt) override;
	void OnUpdateTransformComp(uint32_t rawEnt, const mono::ProxyTransformComp& proxy) override;

	void OnAddMeshComp(uint32_t rawEnt) override;
	void OnRemoveMeshComp(uint32_t rawEnt) override;
	void OnUpdateMeshComp(uint32_t rawEnt, const mono::ProxyMeshComp& proxy) override;
};