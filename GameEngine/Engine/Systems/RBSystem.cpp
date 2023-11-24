#include "PxPhysicsAPI.h"
#include "RBSystem.h"
#include "EngineContext.h"
#include "../Components/RBodyComponent.h"
#include "../Tools/ErrorLogger.h"
#include "../Components/TransformComponent.h"
#include "../Events/SyOnCreateRBodyEvent.h"


using namespace physx;

PxPhysics* SyRBodyComponent::_physics;
PxScene* SyRBodyComponent::_scene;

SyResult SyRBodySystem::Init() 
{
	allocator = std::make_shared<PxDefaultAllocator>();
	errorCallback = std::make_shared<PxDefaultErrorCallback>();
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, *errorCallback);
	if (_foundation == nullptr)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_CRITICAL, "PxCreate_foundation returned nullptr. ");
		exit(-1);
	}
	_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, PxTolerancesScale(), true, nullptr);
	SyRBodyComponent::_physics = _physics;
	if (_physics == nullptr)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_CRITICAL, "PxCreatePhysics returned nullptr. ");
		exit(-1);
	}
	PxSceneDesc sceneDesc(_physics->getTolerancesScale());
	sceneDesc.gravity = GRAVITY;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	_scene = _physics->createScene(sceneDesc);
	_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	SyRBodyComponent::_scene = _scene;
	return SyResult();
}

SyResult SyRBodySystem::Run()
{
	SyResult result;
	auto deltaTime = ServiceLocator::instance()->Get<EngineContext>()->deltaTime;
	if (deltaTime == 0)
	{
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = "EngineContext.deltaTime == 0";
		return result;
	}
	//initialize components that were created at this frame
	auto eventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyOnCreateRBodyEvent);
	for (auto& eventEntity : eventView)
	{
		auto entity = _ecs->get<SyOnCreateRBodyEvent>(eventEntity).Entity;
		auto* rbComponent = _ecs->try_get<SyRBodyComponent>(entity);
		if (rbComponent == nullptr)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "RigidBody Component has not been attached to entity (%d). Something went wrong", (int)entity);
			continue;
		}
		auto* tComponent = _ecs->try_get<TransformComponent>(entity);
		if (tComponent == nullptr)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Entity (%d) is missing the Transform Component. Hence, you can't attach RigidBody Component to it. The RigidBody Component has been removed.", (int)entity);
			_ecs->remove<SyRBodyComponent>(entity);
			continue;
		}
		if (InitComponent(entity, *rbComponent, *tComponent).code != SY_RESCODE_OK)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to initialize RigidBody Component on entity (%d). The RigidBody Component has been removed.", (int)entity);
			_ecs->remove<SyRBodyComponent>(entity);
		}
	}
	//update PhysX according to changes in components member values
	auto view = _ecs->view<SyRBodyComponent, TransformComponent>();
	for (auto& entity : view)
	{
		SyRBodyComponent& rbComponent = view.get<SyRBodyComponent>(entity);
		TransformComponent& trComponent = view.get<TransformComponent>(entity);
		if (rbComponent.RbType == STATIC)
			continue;
		if (rbComponent._rbActor == nullptr)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = "rbComponent.rbActor is nullptr.";
			_ecs->remove<SyRBodyComponent>(entity);
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, result.message.ToString());
			continue;
		}
		PxRigidDynamic* rb = rbComponent._rbActor->is<PxRigidDynamic>();
		PxTransform rbTransform = rb->getGlobalPose();
		if (SyVector3(rbTransform.p) != trComponent.localPosition || SyVector3::PxQuatToEuler(rbTransform.q) != trComponent.localRotation)
			rb->setGlobalPose(PxTransform(trComponent._position,
				SyVector3::EulerToPxQuat(trComponent._rotation)));
		SyVector3 pxLinearVelocity = rb->getLinearVelocity();
		if (rbComponent.LinearVelocity != pxLinearVelocity)
			rb->setLinearVelocity(rbComponent.LinearVelocity);
		SyVector3 pxAngularVelocity = rb->getAngularVelocity();
		if (rbComponent.AngularVelocity != pxAngularVelocity)
			rb->setAngularVelocity(rbComponent.AngularVelocity);
		float pxMass = rb->getMass();
		if (rbComponent.Mass != pxMass)
			rb->setMass(rbComponent.Mass);
		if (rbComponent.Flags & SyERBodyFlags::KINEMATIC)
			rb->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		if (rbComponent.Flags & SyERBodyFlags::DISABLE_GRAVITY)
			rbComponent._rbActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		rbComponent._rbActor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
	}
	//do not simulate if in pause mode
	if(ServiceLocator::instance()->Get<EngineContext>()->playModeState != EngineContext::EPlayModeState::PlayMode)
		return result;
	//advance simulation for 1 frame
	if (!_scene->simulate(deltaTime))
	{
		result.code = SY_RESCODE_ERROR;
		result.message = "Physics simulate function returned false.";
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, result.message.ToString());
		return result;
	}
	if (!_scene->fetchResults(true))
	{
		result.code = SY_RESCODE_ERROR;
		result.message = "Physics fetchResult function returned false.";
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, result.message.ToString());
		return result;
	}
	//update components member values according to simulation results  
	for (auto& entity : view)
	{
		SyRBodyComponent& rbComponent = view.get<SyRBodyComponent>(entity);
		TransformComponent& trComponent = view.get<TransformComponent>(entity);
		if (rbComponent.RbType == STATIC)
			continue;
		if (rbComponent._rbActor == nullptr)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = "rbComponent.rbActor is nullptr.";
			_ecs->remove<SyRBodyComponent>(entity);
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, result.message.ToString());
			continue;
		}
		PxRigidDynamic* rb = rbComponent._rbActor->is<PxRigidDynamic>();
		
		PxTransform rbTrasform = rb->getGlobalPose();
		trComponent.localPosition = rbTrasform.p;
		trComponent.localRotation = SyVector3::PxQuatToEuler(rbTrasform.q);

		rbComponent.LinearVelocity = rb->getLinearVelocity();
		rbComponent.AngularVelocity = rb->getAngularVelocity();
	}
	return SyResult();
}

SyResult SyRBodySystem::Destroy()
{
	PX_RELEASE(_physics);
	PX_RELEASE(_foundation);
	return SyResult();
}

SyResult SyRBodySystem::InitComponent(const entt::entity& entity, SyRBodyComponent& rbComponent, TransformComponent& tComponent)
{
	SyResult result;
	switch (rbComponent.RbType)
	{
	case STATIC: rbComponent._rbActor = _physics->createRigidStatic(PxTransform(tComponent._position, 
														SyVector3::EulerToPxQuat(tComponent._rotation)));
		break;
	case DYNAMIC: rbComponent._rbActor = _physics->createRigidDynamic(PxTransform(tComponent._position, 
														SyVector3::EulerToPxQuat(tComponent._rotation)));
		if (PxRigidBodyExt::setMassAndUpdateInertia(*static_cast<PxRigidBody*>(rbComponent._rbActor), rbComponent.Mass)  == false)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidBodyExt::setMassAndUpdateInertia returned false");
			return result;
		}
		break;
	default:
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Unknown Rigid Body type in SyRbComponent. Entity: %d.", (unsigned)entity);
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("Unknown Rigid Body type in SyRbComponent. Entity: %d.", (unsigned)entity);
		return result;
	};
	if (_scene->addActor(*rbComponent._rbActor) == false)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("scene->addActor returned false for RigidBody component on entity %d", (int)entity);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "scene->addActor returned false for RigidBody component on entity %d", (int)entity);
	};
	return result;
}
