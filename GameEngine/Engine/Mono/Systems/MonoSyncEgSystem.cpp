#include "MonoSyncEgSystem.h"

#include "../SyMono.h"
#include "../../Features/Common/Events/CompRemovedEv.h"


SyResult MonoSyncEgSystem::Init()
{
	auto mono = ServiceLocator::instance()->Get<mono::SyMono>();
	_monoEcs = mono->GetEcs();

	return {};
}

SyResult MonoSyncEgSystem::Run()
{
	OPTICK_EVENT()

	if (!_monoEcs->IsValid())
		return {};

	auto compRemovedView = SY_GET_THIS_FRAME_EVENT_VIEW(CompRemovedEv);
	for (auto ent : compRemovedView)
	{
		auto ev = compRemovedView.get<CompRemovedEv>(ent);
		if (!ev.IsFromMono)
			_monoEcs->EgRemoveComp.Invoke(static_cast<uint32_t>(ev.Ent), ev.Id);
	}

	_monoEcs->TrySendAll();

	return {};
}

SyResult MonoSyncEgSystem::Destroy()
{
	return {};
}