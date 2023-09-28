#include "PxPhysicsAPI.h"
#include "PhysicsSystem.h"
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
	if (foundation == nullptr)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_CRITICAL, L"PxCreateFoundation returned nullptr. ");
		exit(-1);
	}
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, nullptr);
	if (physics == nullptr)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_CRITICAL, L"PxCreatePhysics returned nullptr. ");
		exit(-1);
	}
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
	if(!scene->simulate(deltaTime))
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, L"Physics simulate function returned false. ");
	if(!scene->fetchResults(true))
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, L"Physics fetchResult function returned false. ");
	auto view = ServiceLocator::instance()->Get<EngineContext>()->
		scene->registry.view<SyRBodyComponent, TransformComponent>();
	for (auto& entity : view)
	{
		SyRBodyComponent& rbComponent = view.get<SyRBodyComponent>(entity);
		TransformComponent& trComponent = view.get<TransformComponent>(entity);
		if (rbComponent.rbType == RB_TYPE_STATIC)
			continue;
		if (rbComponent.rbActor == nullptr)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, L"rbComponent.rbActor is nullptr.");
			continue;
		}
		PxRigidDynamic* rb = rbComponent.rbActor->is<PxRigidDynamic>();
		PxTransform rbTrasform = rb->getGlobalPose();
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
