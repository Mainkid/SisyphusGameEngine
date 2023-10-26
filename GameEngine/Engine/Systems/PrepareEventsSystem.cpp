#include "PrepareEventsSystem.h"

SyResult SyPrepareEventsSystem::Init()
{
    SY_EL->RegisterChannel("EVSY");
    return SyResult();
}

SyResult SyPrepareEventsSystem::Run()
{
    auto thisFrameView = _ecs->view<SyEventTag, SyThisFrameEventTag>();
    auto i = thisFrameView.size_hint();
    _ecs->destroy(thisFrameView.begin(), thisFrameView.end());
    i = thisFrameView.size_hint();
    auto nextFrameView = _ecs->view<SyEventTag>();
    auto ii = nextFrameView.size();
    for (auto& entity : nextFrameView)
        _ecs->emplace<SyThisFrameEventTag>(entity);
    return SyResult();
}

SyResult SyPrepareEventsSystem::Destroy()
{
    return SyResult();
}
