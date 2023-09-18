#pragma once
#include <memory>
#include "ISystem.h"
#include "../Tools/Data/Vector.h"
namespace physx
{
    class PxDefaultAllocator;
    class PxDefaultErrorCallback;
    class PxFoundation;
    class PxPhysics;
    class PxScene;
}

class SyPhysicsSystem :
    public ISystem
{
    std::shared_ptr<physx::PxDefaultAllocator>		allocator = nullptr;
    std::shared_ptr<physx::PxDefaultErrorCallback>	errorCallback = nullptr;
    physx::PxFoundation*                            foundation = nullptr;

public:
    physx::PxPhysics*                               physics = nullptr;
    physx::PxScene*                                 scene = nullptr;
    SyVector3                                       gravity = { 0.0f, -9.81f, 0.0f };
    float                                           stepsPerSecond = 60.0f;

    void Init();
    void Run();
    void Destroy();

};

