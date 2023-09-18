#include "PhysicsSystem.h"

using namespace physx;

void PhysicsSystem::Init()
{
	psFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, psAllocator, psErrorCallback);
	psPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *psFoundation, PxTolerancesScale(), true, nullptr);
	PxSceneDesc sceneDesc(psPhysics->getTolerancesScale());
	sceneDesc.gravity = gravity;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	psScene = psPhysics->createScene(sceneDesc);
	
}

void PhysicsSystem::Run()
{
	psScene->simulate(1 / stepsPerSecond);
	psScene->fetchResults(true);
}

void PhysicsSystem::Destroy()
{
	PX_RELEASE(psScene);
	PX_RELEASE(psPhysics);
	PX_RELEASE(psFoundation);
}
