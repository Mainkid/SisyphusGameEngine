#include "PxPhysicsAPI.h"
#include "PhysicsSystem.h"
#include "../Core/ServiceLocator.h"
#include "EngineContext.h"
#include "../Components/RBodyComponent.h"
#include "../Tools/ErrorLogger.h"

using namespace physx;

PxPhysics*	SyRBodyComponent::physics;
PxScene*	SyRBodyComponent::scene;

void SyPhysicsSystem::Init()
{
	allocator = std::make_shared<PxDefaultAllocator>();
	errorCallback = std::make_shared<PxDefaultErrorCallback>();
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, *errorCallback);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, nullptr);
	SyRBodyComponent::physics = physics;
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = gravity;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);
	SyRBodyComponent::scene = scene;
	
}

void SyPhysicsSystem::Run()
{
	auto deltaTime = ServiceLocator::instance()->Get<EngineContext>()->deltaTime;
	if (deltaTime == 0)
		return;
	int simulationResult = scene->simulate(deltaTime) /*== true 
		? SY_NO_ERROR : SY_GENERIC_ERROR_CRITICAL*/;
	//SyErrorLogger::Log(simulationResult, "PhysicsSystem.cpp", 36);
	int fetchResult = scene->fetchResults(true) /*== true 
		? SY_NO_ERROR : SY_GENERIC_ERROR_CRITICAL*/;
	//SyErrorLogger::Log(fetchResult, "PhysicsSystem.cpp", 39);
	auto view = ServiceLocator::instance()->Get<EngineContext>()->
		scene->registry.view<SyRBodyComponent, TransformComponent>();
	for (auto& entity : view)
	{
		SyRBodyComponent& rbComponent = view.get<SyRBodyComponent>(entity);
		TransformComponent& trComponent = view.get<TransformComponent>(entity);
		if (rbComponent.rbType == RB_TYPE_STATIC)
			return;
		PxRigidDynamic* rb = rbComponent.rbActor->is<PxRigidDynamic>();
		/*int rbSearchResult = rb != nullptr ?
			SY_NO_ERROR : SY_GENERIC_ERROR_CRITICAL;
		SyErrorLogger::Log(rbSearchResult, "PhysicsSystem.cpp", 51);*/
		PxTransform rbTrasform = rb->getGlobalPose();
		//std::cout << rbTrasform.p.x << " " << rbTrasform.p.y << " " << rbTrasform.p.z << "\n";
		trComponent.localPosition = rbTrasform.p;
		trComponent.localRotation = SyVector3::PxQuatToEuler(rbTrasform.q);
	}
}

void SyPhysicsSystem::Destroy()
{
	PX_RELEASE(scene);
	PX_RELEASE(physics);
	PX_RELEASE(foundation);
}
