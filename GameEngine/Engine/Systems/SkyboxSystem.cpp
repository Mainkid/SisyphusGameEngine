#include "SkyboxSystem.h"

#include "../Components/SkyboxComponent.h"
#include "../Core/ServiceLocator.h"
#include "../Contexts/HardwareContext.h"
#include "../Contexts/EngineContext.h"
#include "../Features/Resources/ResourceService.h"
#include "../Scene/GameObjectHelper.h"
#include "../Events/SyImageBasedLightingUpdateEvent.h"

SyResult SkyboxSystem::Init()
{
	_hc = ServiceLocator::instance()->Get<HardwareContext>();
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	_rs = ServiceLocator::instance()->Get<ResourceService>();
	GameObjectHelper::CreateSkybox(_ecs);
	return SyResult();
}

SyResult SkyboxSystem::Run()
{
	OPTICK_EVENT();
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
	return SyResult();
}
