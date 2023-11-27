#include "PxPhysicsAPI.h"
#include "RBSystem.h"
#include "../../../Contexts/EngineContext.h"
#include "../Components/RBodyComponent.h"
#include "../../../Core/Tools/ErrorLogger.h"
#include "../../../Components/TransformComponent.h"
#include "../../../Scene/GameObjectHelper.h"
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
	auto view = _ecs->view<SyRBodyComponent>();
	for (auto& entity : view)
	{
		auto name = GET_ENTITY_NAME_CHAR(_ecs, entity);
		auto& rigidBodyC = _ecs->get<SyRBodyComponent>(entity);
		auto* transformC = _ecs->try_get<TransformComponent>(entity);
		if (transformC == nullptr)
		{
			//TODO
			continue;
		}
		if (rigidBodyC._rbActor == nullptr)
		{
			if (InitComponent(entity, rigidBodyC, *transformC).code == SY_RESCODE_ERROR)
			{
				//TODO
				continue;
			};
		}
		else
			UpdateRigidBodyType(entity, rigidBodyC, *transformC);
		UpdateRigidBodyValues(entity, rigidBodyC, *transformC);
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
		auto name = GET_ENTITY_NAME_CHAR(_ecs, entity);
		auto& rigidBodyC = _ecs->get<SyRBodyComponent>(entity);
		auto* transformC = _ecs->try_get<TransformComponent>(entity);
		if (rigidBodyC.RbType == STATIC)
			continue;
		if (rigidBodyC._rbActor == nullptr)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = "rbComponent.rbActor is nullptr.";
			_ecs->remove<SyRBodyComponent>(entity);
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, result.message.ToString());
			continue;
		}
		PxRigidDynamic* rb = rigidBodyC._rbActor->is<PxRigidDynamic>();
		
		PxTransform rbTrasform = rb->getGlobalPose();
		transformC->_position = rbTrasform.p;
		transformC->_rotation = SyVector3::PxQuatToEuler(rbTrasform.q);
		rigidBodyC.LinearVelocity = rb->getLinearVelocity();
		rigidBodyC.AngularVelocity = rb->getAngularVelocity();
	}
	return SyResult();
}

SyResult SyRBodySystem::Destroy()
{
	PX_RELEASE(_physics);
	PX_RELEASE(_foundation);
	return SyResult();
}

SyResult SyRBodySystem::InitComponent(const entt::entity& entity, SyRBodyComponent& rigidBodyC,
	TransformComponent& transformC)
{
	SyResult result;
	switch (rigidBodyC.RbType)
	{
	case STATIC: rigidBodyC._rbActor = _physics->createRigidStatic(PxTransform(transformC._position, 
														SyVector3::EulerToPxQuat(transformC._rotation)));
		break;
	case DYNAMIC: rigidBodyC._rbActor = _physics->createRigidDynamic(PxTransform(transformC._position, 
														SyVector3::EulerToPxQuat(transformC._rotation)));
		if (PxRigidBodyExt::setMassAndUpdateInertia(*static_cast<PxRigidBody*>(rigidBodyC._rbActor), rigidBodyC.Mass) == false)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = xstring("PxRigidBodyExt::setMassAndUpdateInertia returned false for RigidBody Component on entity (%s). ", GET_ENTITY_NAME_CHAR(_ecs, entity));
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidBodyExt::setMassAndUpdateInertia returned false for RigidBody Component on entity (%s). ", GET_ENTITY_NAME_CHAR(_ecs, entity));
			return result;
		}
		break;
	}
	rigidBodyC._prevFrameRbodyType = rigidBodyC.RbType;
	rigidBodyC._wasActorRecreatedThisFrame = true;
	if (rigidBodyC._rbActor == nullptr)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("Failed to create PxRigidActor in RigidBody Component on entity (%s)",
			GET_ENTITY_NAME_CHAR(_ecs, entity));
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to create PxRigidActor in RigidBody Component on entity (%s)",
			GET_ENTITY_NAME_CHAR(_ecs, entity));
	}
	if (_scene->addActor(*rigidBodyC._rbActor) == false)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("scene->addActor returned false for RigidBody Component on entity (%s). ", GET_ENTITY_NAME_CHAR(_ecs, entity));
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "scene->addActor returned false for RigidBody Component on entity (%s). ", GET_ENTITY_NAME_CHAR(_ecs, entity));
	};
	return result;
}

SyResult SyRBodySystem::UpdateRigidBodyType(const entt::entity& entity, SyRBodyComponent& rigidBodyC, TransformComponent& transformC)
{
	SyResult result;
	if (rigidBodyC._prevFrameRbodyType == rigidBodyC.RbType)
	{
		rigidBodyC._wasActorRecreatedThisFrame = false;
		return result;
	}
	rigidBodyC._wasActorRecreatedThisFrame = true;
	if (rigidBodyC._rbActor != nullptr)
		rigidBodyC._rbActor->release();
	switch (rigidBodyC.RbType)
	{
	case STATIC: rigidBodyC._rbActor = _physics->createRigidStatic(PxTransform(transformC._position, 
														SyVector3::EulerToPxQuat(transformC._rotation)));
		break;
	case DYNAMIC: rigidBodyC._rbActor = _physics->createRigidDynamic(PxTransform(transformC._position, 
														SyVector3::EulerToPxQuat(transformC._rotation)));
		if (PxRigidBodyExt::setMassAndUpdateInertia(*static_cast<PxRigidBody*>(rigidBodyC._rbActor), rigidBodyC.Mass)  == false)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = xstring("PxRigidBodyExt::setMassAndUpdateInertia returned false for RigidBody Component on entity (%s). ", GET_ENTITY_NAME_CHAR(_ecs, entity));
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidBodyExt::setMassAndUpdateInertia returned false for RigidBody Component on entity (%s). ", GET_ENTITY_NAME_CHAR(_ecs, entity));
			return result;
		}
		break;
	};
	if (_scene->addActor(*rigidBodyC._rbActor) == false)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("scene->addActor returned false for RigidBody Component on entity (%s). ", GET_ENTITY_NAME_CHAR(_ecs, entity));
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "scene->addActor returned false for RigidBody Component on entity (%s). ", GET_ENTITY_NAME_CHAR(_ecs, entity));
	};
	return result;
}

SyResult SyRBodySystem::UpdateRigidBodyValues(const entt::entity& entity, SyRBodyComponent& rigidBodyC,
	TransformComponent& transformC)
{
	PxRigidDynamic* rbDyn = rigidBodyC._rbActor->is<PxRigidDynamic>();
	if (rbDyn != nullptr)
	{
		PxTransform rbTransform = rbDyn->getGlobalPose();
		if (SyVector3(rbTransform.p) != transformC.localPosition || SyVector3::PxQuatToEuler(rbTransform.q) != transformC.localRotation)
			rbDyn->setGlobalPose(PxTransform(transformC._position,
				SyVector3::EulerToPxQuat(transformC._rotation)));
		SyVector3 pxLinearVelocity = rbDyn->getLinearVelocity();
		if (rigidBodyC.LinearVelocity != pxLinearVelocity)
			rbDyn->setLinearVelocity(rigidBodyC.LinearVelocity);
		SyVector3 pxAngularVelocity = rbDyn->getAngularVelocity();
		if (rigidBodyC.AngularVelocity != pxAngularVelocity)
			rbDyn->setAngularVelocity(rigidBodyC.AngularVelocity);
		float pxMass = rbDyn->getMass();
		if (rigidBodyC.Mass != pxMass)
			rbDyn->setMass(rigidBodyC.Mass);
		auto pxRbFlags = rbDyn->getRigidBodyFlags();
		if ((rigidBodyC.Flags & SyERBodyFlags::KINEMATIC) != ((bool)(rbDyn->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)))
			rbDyn->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rigidBodyC.Flags & SyERBodyFlags::KINEMATIC);
		if ((rigidBodyC.Flags & SyERBodyFlags::DISABLE_GRAVITY) != ((bool)(rigidBodyC._rbActor->getActorFlags() & PxActorFlag::eDISABLE_GRAVITY)))
			rigidBodyC._rbActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, (rigidBodyC.Flags & SyERBodyFlags::DISABLE_GRAVITY));
	}
	else
	{
		PxRigidStatic* rbSt = rigidBodyC._rbActor->is<PxRigidStatic>();
		PxTransform rbTransform = rbSt->getGlobalPose();
		if (SyVector3(rbTransform.p) != transformC.localPosition || SyVector3::PxQuatToEuler(rbTransform.q) != transformC.localRotation)
			rbSt->setGlobalPose(PxTransform(transformC._position,
				SyVector3::EulerToPxQuat(transformC._rotation)));
	}
	rigidBodyC._rbActor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
	return SyResult();
}
