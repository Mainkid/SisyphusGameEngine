#include "PhysicsSystem.h"
#include "PxPhysicsAPI.h"
using namespace physx;

void PhysicsSystem::Init()
{
	pxAllocator = new PxDefaultAllocator;
	pxErrorCallback = new PxDefaultErrorCallback;
	pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *pxAllocator, *pxErrorCallback);
	pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale(), true, nullptr);
	PxSceneDesc sceneDesc(pxPhysics->getTolerancesScale());
	sceneDesc.gravity = gravity;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	psScene = pxPhysics->createScene(sceneDesc);
	
}

void PhysicsSystem::Run()
{
	psScene->simulate(1 / stepsPerSecond);
	psScene->fetchResults(true);
}

void PhysicsSystem::Destroy()
{
	PX_RELEASE(psScene);
	PX_RELEASE(pxPhysics);
	PX_RELEASE(pxFoundation);
}
