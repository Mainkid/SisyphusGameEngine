#include "MonoSyncEgSystem.h"

#include "../SyMono.h"
#include "../../Components/SkyboxComponent.h"
#include "../../Features/Mesh/Components/MeshComponent.h"
#include "../../Components/TransformComponent.h"
#include "../Ecs/SyMonoEcsSyncCollider.h"
#include "../Ecs/SyMonoEcsSyncLight.h"
#include "../Ecs/SyMonoEcsSyncMesh.h"
#include "../Ecs/SyMonoEcsSyncParticles.h"
#include "../Ecs/SyMonoEcsSyncRigid.h"
#include "../Ecs/SyMonoEcsSyncSceneObject.h"
#include "../Ecs/SyMonoEcsSyncSkybox.h"
#include "../Ecs/SyMonoEcsSyncSound.h"
#include "../Ecs/SyMonoEcsSyncTransform.h"


SyResult MonoSyncEgSystem::Init()
{
	auto mono = ServiceLocator::instance()->Get<mono::SyMono>();
	_monoEcs = mono->GetEcs();

	return {};
}

SyResult MonoSyncEgSystem::Run()
{
	if (!_monoEcs->IsValid())
		return {};

	_monoEcs->TrySendAll();

	return {};
}

SyResult MonoSyncEgSystem::Destroy()
{
	return {};
}