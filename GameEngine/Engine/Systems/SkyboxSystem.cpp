#include "SkyboxSystem.h"

#include "../Components/SkyboxComponent.h"
#include "../Core/ServiceLocator.h"
#include "../Systems/HardwareContext.h"
#include "../Systems/EngineContext.h"
#include "../Systems/ResourceService.h"
#include "../Scene/GameObjectHelper.h"
#include "../Core/ECS/Events/SyImageBasedLightingUpdateEvent.h"

SyResult SkyboxSystem::Init()
{
	_hc = ServiceLocator::instance()->Get<HardwareContext>();
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	_rs = ServiceLocator::instance()->Get<ResourceService>();
	GameObjectHelper::CreateSkybox(_ecs);
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Skybox system initialization successful.");
	return SyResult();
}

SyResult SkyboxSystem::Run()
{
	auto view = _ecs->view<SkyboxComponent>();
	for (auto& entity : view)
	{
		SkyboxComponent& sc = _ecs->get<SkyboxComponent>(entity);
		auto sc_hash = _hasher(sc.uuid);
		if (sc.Hash!= sc_hash)
		{
			sc.SkyboxRes = std::static_pointer_cast<SkyboxResource>(_rs->LoadResource(sc.uuid));
			CallEvent<SyImageBasedLightingUpdateEvent>("SyImageBasedLightingUpdateEvent");
			sc.Hash = _hasher(sc.uuid);
		}
	}
	return SyResult();
}

SyResult SkyboxSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Skybox system destruction successful.");
	return SyResult();
}