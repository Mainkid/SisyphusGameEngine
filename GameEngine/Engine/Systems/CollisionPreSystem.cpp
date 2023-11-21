#include "CollisionPreSystem.h"
#include "../Components/ColliderComponent.h"
#include "PxPhysicsAPI.h"
#include "../Components/RBodyComponent.h"

SyResult SyCollisionPreSystem::Init()
{
    return SyResult();
}

SyResult SyCollisionPreSystem::Run()
{
    // auto pView = _ecs->view<SyPrimitiveColliderComponent, SyRBodyComponent>();
    // for (auto& entity : pView)
    // {
    //     if (_ecs->try_get<SyEventOnCreateCollider>(entity) != nullptr)
    //         continue; //skip uninitialized components
    //     bool isVisualizationEnabled = true/*(_ecs->try_get<SyTagColliderVisualizationEnabled>(entity) != nullptr)*/;
    //     _ecs->get<SyRBodyComponent>(entity)._rbActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, isVisualizationEnabled);
    //     _ecs->get<SyPrimitiveColliderComponent>(entity)._shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, isVisualizationEnabled);
    // }
    // auto tMView = _ecs->view<SyTrimeshColliderComponent, SyRBodyComponent>();
    // for (auto& entity : tMView)
    // {
    //     if (_ecs->try_get<SyEventOnCreateCollider>(entity) != nullptr)
    //         continue; //skip uninitialized components
    //     bool isVisualizationEnabled = true/*(_ecs->try_get<SyTagColliderVisualizationEnabled>(entity) != nullptr)*/;
    //     _ecs->get<SyRBodyComponent>(entity)._rbActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, isVisualizationEnabled);
    //     _ecs->get<SyPrimitiveColliderComponent>(entity)._shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, isVisualizationEnabled);
    // }
    return SyResult();
}

SyResult SyCollisionPreSystem::Destroy()
{
    return SyResult();
}
