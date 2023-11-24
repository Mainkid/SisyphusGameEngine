#include "CollisionPreSystem.h"

#include "EngineContext.h"
#include "../Components/ColliderComponent.h"
#include "PxPhysicsAPI.h"
#include "../Components/RBodyComponent.h"
#include "../Core/ECS/Event.h"

SyResult SyCollisionPreSystem::Init()
{
    return SyResult();
}

SyResult SyCollisionPreSystem::Run()
{

    // for (const auto& entity : ServiceLocator::instance()->Get<EngineContext>()->hudData.selectedEntityIDs)
    // {
    //     auto* rbComponent = _ecs->try_get<SyRBodyComponent>(entity);
    //     if (rbComponent != nullptr
    // }
    //
    return SyResult();
}

SyResult SyCollisionPreSystem::Destroy()
{
    return SyResult();
}
