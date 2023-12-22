#pragma once
#include "../Core/IService.h"
//#include "PxPhysicsAPI.h"
namespace  physx
{
    class PxPhysics;
    class PxScene;
}
struct SyPhysicsContext : IService
{
    physx::PxPhysics* Physics = nullptr;
};
