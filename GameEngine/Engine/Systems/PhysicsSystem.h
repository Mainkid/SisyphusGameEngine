#pragma once
#include "ISystem.h"
#include "../../vendor/PhysX/PxPhysicsAPI.h"

//using namespace physx;



class PhysicsSystem :
    public ISystem
{
    physx::PxDefaultAllocator		psAllocator;
    physx::PxDefaultErrorCallback	psErrorCallback;
    physx::PxFoundation*            psFoundation = NULL;

public:
    physx::PxPhysics*               psPhysics = NULL;
    physx::PxScene*                 psScene = NULL;
    physx::PxVec3                   gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    physx::PxReal                   stepsPerSecond = 60.0f;

    void Init();
    void Run();
    void Destroy();

};

