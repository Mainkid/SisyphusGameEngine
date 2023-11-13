#include "PxPhysicsAPI.h"
#include "RBSystem.h"
#include "EngineContext.h"
#include "../Components/RBodyComponent.h"
#include "../Tools/ErrorLogger.h"
#include "../Components/TransformComponent.h"
#include "../Tools/MathHelper.h"


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
	sceneDesc.gravity = _gravity;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	_scene = _physics->createScene(sceneDesc);
	SyRBodyComponent::_scene = _scene;
	return SyResult();
}

SyResult SyRBodySystem::Run()
{
	SyResult result;
	static bool wasPreviousFrameInPause = false;
	if(ServiceLocator::instance()->Get<EngineContext>()->playModeState != EngineContext::EPlayModeState::PlayMode)
	{
		wasPreviousFrameInPause = true;
		return SyResult();
	}
	auto deltaTime = ServiceLocator::instance()->Get<EngineContext>()->deltaTime;
	if (deltaTime == 0)
	{
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = "EngineContext.deltaTime == 0";
		return result;
	}
	auto toInitView = _ecs->view<SyRBodyComponent, TransformComponent, SyRbCreateOnNextUpdateTag>();
	for (auto& entity : toInitView)
	{
		auto& rbComponent = _ecs->get<SyRBodyComponent>(entity);
		auto& tComponent = _ecs->get<TransformComponent>(entity);
		if (InitComponent(entity, rbComponent, tComponent).code != SY_RESCODE_OK)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to initialize RigidBody Component on entity %d", (int)entity);
			_ecs->remove<SyRBodyComponent>(entity);
		}
		_ecs->remove<SyRbCreateOnNextUpdateTag>(entity);
	}
	auto view = _ecs->view<SyRBodyComponent, TransformComponent>();
	if (wasPreviousFrameInPause == true)
	{
		for (auto& entity : view)
		{
			SyRBodyComponent& rbComponent = view.get<SyRBodyComponent>(entity);
			TransformComponent& trComponent = view.get<TransformComponent>(entity);
			if (rbComponent._rbType == STATIC)
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
			if (SyVector3(rbTrasform.p) != trComponent.localPosition || SyVector3::PxQuatToEuler(rbTrasform.q) != trComponent.localRotation)
			{
				rb->setGlobalPose(PxTransform(trComponent._position,
					SyVector3::EulerToPxQuat(trComponent._rotation)));
			}
		}
		wasPreviousFrameInPause = false;
	}
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
	for (auto& entity : view)
	{
		SyRBodyComponent& rbComponent = view.get<SyRBodyComponent>(entity);
		TransformComponent& trComponent = view.get<TransformComponent>(entity);
		if (rbComponent._rbType == STATIC)
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
	switch (rbComponent._rbType)
	{
	case STATIC: rbComponent._rbActor = _physics->createRigidStatic(PxTransform(tComponent._position, 
														SyVector3::EulerToPxQuat(tComponent._rotation)));
		break;
	case DYNAMIC: rbComponent._rbActor = _physics->createRigidDynamic(PxTransform(tComponent._position, 
														SyVector3::EulerToPxQuat(tComponent._rotation)));
		break;
	default:
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Unknown Rigid Body type in SyRbComponent. Entity: %d.", (unsigned)entity);
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("Unknown Rigid Body type in SyRbComponent. Entity: %d.", (unsigned)entity);
		return result;
	};
	if (rbComponent._rbType == DYNAMIC)
	{
		bool updateMassResult = PxRigidBodyExt::setMassAndUpdateInertia(	*static_cast<PxRigidBody*>(rbComponent._rbActor),
																			rbComponent._mass);
		if (updateMassResult == false)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidBodyExt::setMassAndUpdateInertia returned false");
			return result;
		}
	}
	if (rbComponent._flags & SyERBodyFlags::KINEMATIC)
	{
		PxRigidBody* rb = rbComponent._rbActor->is<PxRigidBody>();
		rb->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	}
	if (_scene->addActor(*rbComponent._rbActor) == false)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("scene->addActor returned false for RigidBody component on entity %d", (int)entity);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "scene->addActor returned false for RigidBody component on entity %d", (int)entity);
	};
	return result;
}
