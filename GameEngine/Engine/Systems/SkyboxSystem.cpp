#include "SkyboxSystem.h"

#include "../Components/SkyboxComponent.h"
#include "../Core/ServiceLocator.h"
#include "../Systems/HardwareContext.h"
#include "../Systems/EngineContext.h"
#include "../Systems/ResourceService.h"

SyResult SkyboxSystem::Init()
{
	_hc = ServiceLocator::instance()->Get<HardwareContext>();
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	_rs = ServiceLocator::instance()->Get<ResourceService>();
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Skybox system initialization successful.");
	return SyResult();
}

SyResult SkyboxSystem::Run()
{
	auto view = _ecs->view<SkyboxComponent>();
	for (auto& entity : view)
	{
		
	}
	return SyResult();
}

SyResult SkyboxSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Skybox system destruction successful.");
	return SyResult();
}
