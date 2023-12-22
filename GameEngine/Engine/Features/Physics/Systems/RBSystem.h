#pragma once
#include <memory>

#include "../../../Core/EngineCore.h"
#include "../../../Core/ECS/SystemBase.h"

#define GET_PHYSICS_CONTEXT ServiceLocator::instance()->Get<SyPhysicsContext>()

#pragma region forward declaration
namespace physx
{
    class PxDefaultAllocator;
    class PxDefaultErrorCallback;
    class PxFoundation;
    class PxPhysics;
    class PxScene;
}
struct SyRigidBodyComponent;
#pragma endregion

class SyRigidBodySystem :
    public SystemBase
{
    std::shared_ptr<physx::PxDefaultAllocator>		_allocator = nullptr;
    std::shared_ptr<physx::PxDefaultErrorCallback>	_errorCallback = nullptr;
    physx::PxFoundation* _foundation = nullptr;
    
    physx::PxPhysics*                               _physics = nullptr;
    physx::PxScene*                                 _scene = nullptr;
    const SyVector3                                 GRAVITY = { 0.0f, -9.81f, 0.0f };
    float                                           _stepsPerSecond = 60.0f;
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;

private:
    SyResult InitComponent(const entt::entity& entity, SyRigidBodyComponent& rigidBodyC, TransformComponent& transformC);
    SyResult UpdateRigidBodyType(const entt::entity& entity, SyRigidBodyComponent& rigidBodyC, TransformComponent& transformC);
    SyResult UpdateRigidBodyValues(const entt::entity& entity, SyRigidBodyComponent& rigidBodyC, TransformComponent& transformC);

    std::vector<entt::entity> GetJointOtherEntities(const entt::entity& entity);
    friend class SyCollisionSystem;
    friend class SyRBodySimulationSystem;
};

