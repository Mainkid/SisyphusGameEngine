#include "MonoHotReloadSystem.h"

#include "../SyMono.h"
#include "../../Events/SyHotReloadEvent.h"
#include "../../Events/SyPlayModeEndedEvent.h"

SyResult MonoHotReloadSystem::Init()
{
	_mono = ServiceLocator::instance()->Get<mono::SyMono>();

	_mono->GetEcs()->BindEcs(_ecs);
	_mono->GetEditor()->BindEcs(_ecs);

	return {};
}

SyResult MonoHotReloadSystem::Run()
{
	bool isHotReloadRequested = false;

	if (!_isInitialHotReloadDone)
	{
		_mono->HotReload();
		_isInitialHotReloadDone = true;
	}

	auto hotReloadView = SY_GET_THIS_FRAME_EVENT_VIEW(SyHotReloadEvent);
	isHotReloadRequested |= hotReloadView.size_hint() > 0;

	auto playModeEndView = SY_GET_THIS_FRAME_EVENT_VIEW(SyPlayModeEndedEvent);
	isHotReloadRequested |= playModeEndView.size_hint() > 0;

	if (!isHotReloadRequested)
		return {};

	auto view = _ecs->view<GameObjectComp>();
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