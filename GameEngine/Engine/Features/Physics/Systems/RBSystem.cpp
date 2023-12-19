#include "PxPhysicsAPI.h"
#include "RBSystem.h"
#include "../../../Contexts/EngineContext.h"
#include "../Components/RBodyComponent.h"
#include "../../../Core/Tools/ErrorLogger.h"
#include "../../../Components/TransformComponent.h"
#include "../../../Scene/GameObjectHelper.h"
#include "../../Common/Events/OnAddComponentEvent.h"
#include "../../Common/Events/OnRemoveComponentEvent.h"
#include "../../../Core/Tools/Macro.h"
#include "../Components/JointComponent.h"

using namespace physx;

PxPhysics* SyRBodyComponent::_physics;
PxScene* SyRBodyComponent::_scene;

SyResult SyRigidBodySystem::Init() 
{
	_allocator = std::make_shared<PxDefaultAllocator>();
	_errorCallback = std::make_shared<PxDefaultErrorCallback>();
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *_allocator, *_errorCallback);
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

SyResult SyRigidBodySystem::Run()
{
	SyResult result;
	auto deltaTime = ServiceLocator::instance()->Get<EngineContext>()->deltaTime;
	if (deltaTime == 0)
		return result;
	//initialize components that were created at this frame
	auto view = _ecs->view<SyRBodyComponent>();
	for (auto& entity : view)
	{
		auto name = SY_GET_ENTITY_NAME_CHAR(_ecs, entity); //for debug
		auto& rigidBodyC = _ecs->get<SyRBodyComponent>(entity);
		auto* transformC = _ecs->try_get<TransformComponent>(entity);
		if (transformC == nullptr)
		{
			_ecs->emplace<TransformComponent>(entity);
			CallEvent<SyOnAddComponentEvent>("AddTransform", SyEComponentType::TRANSFORM, entity);
			SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
				"Transform Component required for RigidBody Component is missing on entity (%s). The Transform Component has been added in the next frame.",
				SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
			continue;
		}
		if (rigidBodyC._rbActor == nullptr)
		{
			if (InitComponent(entity, rigidBodyC, *transformC).code == SY_RESCODE_ERROR)
			{
				_ecs->remove<SyRBodyComponent>(entity);
				CallEvent<SyOnRemoveComponentEvent>("RemoveRigidBody", SyEComponentType::RIGID_BODY, entity);
				SY_LOG_PHYS(SY_LOGLEVEL_ERROR,
					"Failed to initialize RigidBody Component on entity (%s). The component has been removed.",
					SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
				continue;
			};
		}
		else if (ServiceLocator::instance()->Get<EngineContext>()->playModeState == EngineContext::EPlayModeState::PlayMode)
		{
			UpdateRigidBodyType(entity, rigidBodyC, *transformC);
			UpdateRigidBodyValues(entity, rigidBodyC, *transformC);
		}
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
		auto name = SY_GET_ENTITY_NAME_CHAR(_ecs, entity); //for debug
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
		PxRigidDynamic* rbDyn = rigidBodyC._rbActor->is<PxRigidDynamic>();
		
		PxTransform rbTrasform = rbDyn->getGlobalPose();
		transformC->_position = rbTrasform.p;
		transformC->_rotation = SyVector3::PxQuatToEuler(rbTrasform.q);
		if (rigidBodyC._mustSaveUserVelocity == false)
		{
			rigidBodyC.LinearVelocity = rbDyn->getLinearVelocity();
			rigidBodyC.AngularVelocity = rbDyn->getAngularVelocity();
		}
		rigidBodyC._wasTransformChangedFromOutside = false;
		rigidBodyC._mustSaveUserVelocity = false;
		rbDyn->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rigidBodyC.Flags & SyERBodyFlags::KINEMATIC);
		
	}
	return SyResult();
}

SyResult SyRigidBodySystem::Destroy()
{
	PX_RELEASE(_physics);
	PX_RELEASE(_foundation);
	return SyResult();
}

SyResult SyRigidBodySystem::InitComponent(const entt::entity& entity, SyRBodyComponent& rigidBodyC,
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
		if (rigidBodyC.Mass < 0)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
						"Negative mass for RigidBody Component detected on entity (%s). It is illegal. Instead, default mass 1 is used. ", 
						SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
			rigidBodyC.Mass = 1.0f;
		}
		if (PxRigidBodyExt::setMassAndUpdateInertia(*static_cast<PxRigidBody*>(rigidBodyC._rbActor), rigidBodyC.Mass) == false)
		{
			SY_PROCESS_PHYS_ERROR(	result,
									"PxRigidBodyExt::setMassAndUpdateInertia returned false for RigidBody Component on entity (%s). ", 
									SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
			return result;
		}
		break;
	}
	rigidBodyC._prevFrameRbodyType = rigidBodyC.RbType;
	rigidBodyC._wasActorRecreatedThisFrame = true;
	if (rigidBodyC._rbActor == nullptr)
	{
		SY_PROCESS_PHYS_ERROR(	result,
								"Failed to create PxRigidActor in RigidBody Component on entity (%s)",
								SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
	}
	if (_scene->addActor(*rigidBodyC._rbActor) == false)
	{
		SY_PROCESS_PHYS_ERROR(	result,
								"scene->addActor returned false for RigidBody Component on entity (%s).",
								SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
	};
	return result;
}

SyResult SyRigidBodySystem::UpdateRigidBodyType(const entt::entity& entity, SyRBodyComponent& rigidBodyC, TransformComponent& transformC)
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
		if (rigidBodyC.Mass < 0)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
						"Negative mass for RigidBody Component detected on entity (%s). It is illegal. Instead, default mass 1 is used. ", 
						SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
			rigidBodyC.Mass = 1.0f;
		}
		if (PxRigidBodyExt::setMassAndUpdateInertia(*static_cast<PxRigidBody*>(rigidBodyC._rbActor), rigidBodyC.Mass) == false)
		{
			SY_PROCESS_PHYS_ERROR(	result,
									"PxRigidBodyExt::setMassAndUpdateInertia returned false for RigidBody Component on entity (%s). ", 
									SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
			return result;
		}
		break;
	};
	if (_scene->addActor(*rigidBodyC._rbActor) == false)
	{
		SY_PROCESS_PHYS_ERROR(	result,
								"scene->addActor returned false for RigidBody Component on entity (%s).",
								SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
	};
	return result;
}

SyResult SyRigidBodySystem::UpdateRigidBodyValues(const entt::entity& entity, SyRBodyComponent& rigidBodyC,
	TransformComponent& transformC)
{
	PxRigidDynamic* rbDyn = rigidBodyC._rbActor->is<PxRigidDynamic>();
	if (rbDyn != nullptr)
	{
		PxTransform rbTransform = rbDyn->getGlobalPose();
		if (SyVector3(rbTransform.p) != transformC._position ||
			!SyMathHelper::AreEqual(SyVector3::PxQuatToEuler(rbTransform.q), transformC._rotation))
		{
			rigidBodyC._wasTransformChangedFromOutside = true;
			rigidBodyC._mustSaveUserVelocity = true;
			auto otherEntities = GetJointOtherEntities(entity);
			for (auto& anotherEntity : otherEntities)
			{
				SyRBodyComponent* anotherRBodyCPtr = _ecs->try_get<SyRBodyComponent>(anotherEntity);
				if (anotherRBodyCPtr != nullptr)
					anotherRBodyCPtr->_mustSaveUserVelocity = true;
				else
				SY_LOG_PHYS(SY_LOGLEVEL_DEBUG,
					"No rigid body was found on entity (%s) with a joint atached. That is weird",
					SY_GET_ENTITY_NAME_CHAR(_ecs, anotherEntity));
			}
			auto* jointCHelperPtr = _ecs->try_get<SyJointComponentHelper>(entity);
			if (jointCHelperPtr != nullptr)
			{
				SyRBodyComponent* otherRBodyCPtr = _ecs->try_get<SyRBodyComponent>(jointCHelperPtr->jointHolder);
				if (otherRBodyCPtr != nullptr)
					otherRBodyCPtr->_mustSaveUserVelocity = true;
				else
					SY_LOG_PHYS(SY_LOGLEVEL_DEBUG,
						"No rigid body was found on entity (%s) with a joint atached. That is weird",
						SY_GET_ENTITY_NAME_CHAR(_ecs, jointCHelperPtr->jointHolder));
			}
			rbDyn->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			rbDyn->setKinematicTarget(PxTransform(	transformC._position,
													SyVector3::EulerToPxQuat(transformC._rotation)));
			return SyResult();
		}

		SyVector3 pxLinearVelocity = rbDyn->getLinearVelocity();
		if (rigidBodyC.LinearVelocity != pxLinearVelocity)
			rbDyn->setLinearVelocity(rigidBodyC.LinearVelocity);
		SyVector3 pxAngularVelocity = rbDyn->getAngularVelocity();
		if (rigidBodyC.AngularVelocity != pxAngularVelocity)
			rbDyn->setAngularVelocity(rigidBodyC.AngularVelocity);
		float pxMass = rbDyn->getMass();
		if (rigidBodyC.Mass != pxMass)
			rbDyn->setMass(rigidBodyC.Mass);
		if ((rigidBodyC.Flags & SyERBodyFlags::KINEMATIC) != ((bool)(rbDyn->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)))
			rbDyn->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rigidBodyC.Flags & SyERBodyFlags::KINEMATIC);
		if ((rigidBodyC.Flags & SyERBodyFlags::DISABLE_GRAVITY) != ((bool)(rigidBodyC._rbActor->getActorFlags() & PxActorFlag::eDISABLE_GRAVITY)))
			rigidBodyC._rbActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, (rigidBodyC.Flags & SyERBodyFlags::DISABLE_GRAVITY));
	}
	else
	{
		PxRigidStatic* rbSt = rigidBodyC._rbActor->is<PxRigidStatic>();
		PxTransform rbTransform = rbSt->getGlobalPose();
		if (SyVector3(rbTransform.p) != transformC._position || SyVector3::PxQuatToEuler(rbTransform.q) != transformC._rotation)
		{
			rbSt->setGlobalPose(PxTransform(transformC._position,
				SyVector3::EulerToPxQuat(transformC._rotation)));
			rigidBodyC._wasTransformChangedFromOutside = true;
			rigidBodyC._rbActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
			return SyResult();
		}
		else
		{
			rigidBodyC._wasTransformChangedFromOutside = false;
			rigidBodyC._rbActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
		}
	}
	rigidBodyC._rbActor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
	return SyResult();
}

std::vector<entt::entity> SyRigidBodySystem::GetJointOtherEntities(const entt::entity& entity)
{
	std::vector<entt::entity> result;
	auto* fixedJointCPtr = _ecs->try_get<SyFixedJointComponent>(entity);
	if (fixedJointCPtr != nullptr)
		result.push_back(fixedJointCPtr->OtherEntity);
	auto* hingeJointCPtr = _ecs->try_get<SyHingeJointComponent>(entity);
	if (hingeJointCPtr != nullptr)
		result.push_back(hingeJointCPtr->OtherEntity);
	//and other joint types
	return result;
	
}

