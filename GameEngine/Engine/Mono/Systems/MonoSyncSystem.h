#pragma once

#include "../../Core/ECS/SystemBase.h"
#include "../ISyMonoGameCallbackReceiver.h"

struct EngineContext;

namespace mono
{
class SyMonoGame;
}

class MonoSyncSystem : public SystemBase, mono::ISyMonoGameCallbackReceiver
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

private:
	EngineContext* _engineContext = nullptr;
	mono::SyMonoGame* _monoGame = nullptr;

	uint32_t CreateEngineEntity() override;
	void DestroyEngineEntity(uint32_t rawEnt) override;

	void AddTransformComp(uint32_t rawEnt) override;
	void RemoveTransformComp(uint32_t rawEnt) override;
	void UpdateTransformComp(uint32_t rawEnt, const mono::ProxyTransformComp& proxy) override;

	void AddMeshComp(uint32_t rawEnt) override;
	void RemoveMeshComp(uint32_t rawEnt) override;
	void UpdateMeshComp(uint32_t rawEnt, const mono::ProxyMeshComp& proxy) override;
};
