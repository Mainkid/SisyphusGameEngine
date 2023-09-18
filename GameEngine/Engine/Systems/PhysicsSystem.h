#pragma once
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

class PhysicsSystem :
    public ISystem
{
    physx::PxDefaultAllocator*		pxAllocator = nullptr;
    physx::PxDefaultErrorCallback*	pxErrorCallback = nullptr;
    physx::PxFoundation*            pxFoundation = nullptr;

public:
    physx::PxPhysics*               pxPhysics = nullptr;
    physx::PxScene*                 psScene = nullptr;
    SyVector3                       gravity = { 0.0f, -9.81f, 0.0f };
    float                           stepsPerSecond = 60.0f;

    void Init();
    void Run();
    void Destroy();

};

