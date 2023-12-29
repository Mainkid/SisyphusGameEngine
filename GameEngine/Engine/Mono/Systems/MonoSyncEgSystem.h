#pragma once
#include "../../Core/ECS/SystemBase.h"

namespace mono
{
	class SyMonoEcs;
}

class MonoSyncEgSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

private:
	mono::SyMonoEcs* _monoEcs = nullptr;

	void SendSceneObjects();
	void SendTransforms();
	void SendMeshes();
	void SendLights();
	void SendColliders();
	void SendRigids();
	void SendSkyboxes();
	void SendParticles();
};
