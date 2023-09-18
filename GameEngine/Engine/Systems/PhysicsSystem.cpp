#include "PxPhysicsAPI.h"
#include "PhysicsSystem.h"

using namespace physx;

void SyPhysicsSystem::Init()
{
	allocator = std::make_shared<PxDefaultAllocator>();
	errorCallback = std::make_shared<PxDefaultErrorCallback>();
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, *errorCallback);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, nullptr);
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = gravity;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);
	
}

void SyPhysicsSystem::Run()
{
	scene->simulate(1 / stepsPerSecond);
	scene->fetchResults(true);
}

void SyPhysicsSystem::Destroy()
{
	PX_RELEASE(scene);
	PX_RELEASE(physics);
	PX_RELEASE(foundation);
}
