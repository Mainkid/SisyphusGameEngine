#include "PxPhysicsAPI.h"
#include "PhysicsSystem.h"
#include "EngineContext.h"
#include "../Components/RBodyComponent.h"
#include "../Tools/ErrorLogger.h"
#include "../Components/TransformComponent.h"


using namespace physx;

PxPhysics*	SyRBodyComponent::physics;
PxScene*	SyRBodyComponent::scene;

SyResult SyPhysicsSystem::Init() 
{
	allocator = std::make_shared<PxDefaultAllocator>();
	errorCallback = std::make_shared<PxDefaultErrorCallback>();
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, *errorCallback);
	if (foundation == nullptr)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_CRITICAL, "PxCreateFoundation returned nullptr. ");
		exit(-1);
	}
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, nullptr);
	if (physics == nullptr)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_CRITICAL, "PxCreatePhysics returned nullptr. ");
		exit(-1);
	}
	SyRBodyComponent::physics = physics;
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = gravity;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);
	SyRBodyComponent::scene = scene;

	CallEvent<SyTestEvent>("TestEvent", "Test event was called! ");
	SY_LOG_PHYS(SY_LOGLEVEL_INFO, "Physics initialization successful. ");
	return SyResult();
}

SyResult SyPhysicsSystem::Run()
{
	SyResult result;
	auto deltaTime = ServiceLocator::instance()->Get<EngineContext>()->deltaTime;
	if (deltaTime == 0)
	{
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = "EngineContext.deltaTime == 0";
		return result;
	}
	if (!scene->simulate(deltaTime))
	{
		result.code = SY_RESCODE_ERROR;
		result.message = "Physics simulate function returned false.";
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR,  result.message.ToString());
		return result;
	}
	if (!scene->fetchResults(true))
	{
		result.code = SY_RESCODE_ERROR;
		result.message = "Physics fetchResult function returned false.";
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, result.message.ToString());
		return result;
	}
	auto view = _ecs->view<SyRBodyComponent, TransformComponent>();
	for (auto& entity : view)
	{
		SyRBodyComponent& rbComponent = view.get<SyRBodyComponent>(entity);
		TransformComponent& trComponent = view.get<TransformComponent>(entity);
		if (rbComponent.rbType == RB_TYPE_STATIC)
			continue;
		if (rbComponent.rbActor == nullptr)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = "rbComponent.rbActor is nullptr.";
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, result.message.ToString());
			continue;
		}
		PxRigidDynamic* rb = rbComponent.rbActor->is<PxRigidDynamic>();
		PxTransform rbTrasform = rb->getGlobalPose();
		trComponent.localPosition = rbTrasform.p;
		trComponent.localRotation = SyVector3::PxQuatToEuler(rbTrasform.q);
	}

	auto eventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyTestEvent);
	for (auto& entity : eventView)
	{
		SyTestEvent& testEvent = eventView.get<SyTestEvent>(entity);
		SY_LOG_EVSY(SY_LOGLEVEL_WARNING, testEvent.message.c_str());
	}
	return SyResult();
}

SyResult SyPhysicsSystem::Destroy()
{
	PX_RELEASE(scene);
	PX_RELEASE(physics);
	PX_RELEASE(foundation);
	SY_LOG_PHYS(SY_LOGLEVEL_INFO, "Physics system destruction successful. ");
	return SyResult();
}
