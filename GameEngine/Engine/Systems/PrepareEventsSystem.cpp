#include "PrepareEventsSystem.h"

SyResult SyPrepareEventsSystem::Init()
{
    SY_EL->RegisterChannel("EVSY");
    return SyResult();
}

SyResult SyPrepareEventsSystem::Run()
{
    auto thisFrameView = _ecs->view<SyEventTag, SyThisFrameEventTag>();
    _ecs->destroy(thisFrameView.begin(), thisFrameView.end());
    auto nextFrameView = _ecs->view<SyEventTag>();
    for (auto& entity : nextFrameView)
        _ecs->emplace<SyThisFrameEventTag>(entity);
    return SyResult();
}

SyResult SyPrepareEventsSystem::Destroy()
{
    return SyResult();
}
