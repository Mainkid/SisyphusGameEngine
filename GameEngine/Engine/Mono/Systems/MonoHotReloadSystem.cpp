#include "MonoHotReloadSystem.h"

#include "../SyMono.h"
#include "../../Events/SyHotReloadEvent.h"
#include "../Components/MonoSyncComp.h"

SyResult MonoHotReloadSystem::Init()
{
	_mono = ServiceLocator::instance()->Get<mono::SyMono>();

	_mono->HotReload();

	return {};
}

SyResult MonoHotReloadSystem::Run()
{
	auto eventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyHotReloadEvent);
	if (eventView.size_hint() == 0)
		return {};
	
	auto view = _ecs->view<MonoSyncComp>();
	for (auto ent : view)
		_ecs->destroy(ent);

	std::cout << "hot reload start" << std::endl;
	_mono->HotReload();
	std::cout << "hot reload end" << std::endl;

	return {};
}

SyResult MonoHotReloadSystem::Destroy()
{
	return {};
}