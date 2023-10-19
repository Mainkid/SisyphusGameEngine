#pragma once
#include <memory>
#include "../Core/ECS/SystemBase.h"
#include "../Tools/Data/Vector.h"

# pragma region forward declaration
namespace physx
{
    class PxDefaultAllocator;
    class PxDefaultErrorCallback;
    class PxFoundation;
    class PxPhysics;
    class PxScene;
}
#pragma endregion

class SyPhysicsSystem :
    public SystemBase
{
    std::shared_ptr<physx::PxDefaultAllocator>		allocator = nullptr;
    std::shared_ptr<physx::PxDefaultErrorCallback>	errorCallback = nullptr;
    physx::PxFoundation*                            foundation = nullptr;

public:
    physx::PxPhysics*                               physics = nullptr;
    physx::PxScene*                                 scene = nullptr;
    SyVector3                                       gravity = { 0.0f, -9.81f, 0.0f };
    float                                           stepsPerSecond = 60.0f;

    SyResult Init();
    SyResult Run();
    SyResult Destroy();
};

