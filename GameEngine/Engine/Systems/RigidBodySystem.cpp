#include "PxPhysicsAPI.h"
#include "RigidBodySystem.h"
#include "EngineContext.h"
#include "../Components/RBodyComponent.h"
#include "../Tools/ErrorLogger.h"
#include "../Components/TransformComponent.h"


using namespace physx;

PxPhysics* SyRBodyComponent::_physics;
PxScene* SyRBodyComponent::_scene;

SyResult SyRigidBodySystem::Init() 
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
	if (_physics == nullptr)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_CRITICAL, "PxCreatePhysics returned nullptr. ");
		exit(-1);
	}
	SyRBodyComponent::_physics = _physics;
	PxSceneDesc sceneDesc(_physics->getTolerancesScale());
	sceneDesc.gravity = _gravity;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	_scene = _physics->createScene(sceneDesc);
	SyRBodyComponent::_scene = _scene;
	return SyResult();
}

SyResult SyRigidBodySystem::Run()
{
	SyResult result;
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
		auto rbComponent = _ecs->get<SyRBodyComponent>(entity);
		auto tComponent = _ecs->get<TransformComponent>(entity);
		if (InitComponent(entity, rbComponent, tComponent).code != SY_RESCODE_OK)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to initialize RigidBody Component on entity %d", (int)entity);
			_ecs->remove<SyRBodyComponent>(entity);
		}
		_ecs->remove<SyRbCreateOnNextUpdateTag>(entity);
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
	auto view = _ecs->view<SyRBodyComponent, TransformComponent>();
	for (auto& entity : view)
	{
		SyRBodyComponent& rbComponent = view.get<SyRBodyComponent>(entity);
		TransformComponent& trComponent = view.get<TransformComponent>(entity);
		if (rbComponent._rbType == SY_RB_TYPE_STATIC)
			continue;
		if (rbComponent._rbActor == nullptr)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = "rbComponent.rbActor is nullptr.";
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, result.message.ToString());
			continue;
		}
		PxRigidDynamic* rb = rbComponent._rbActor->is<PxRigidDynamic>();
		
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

SyResult SyRigidBodySystem::Destroy()
{
	PX_RELEASE(_scene);
	PX_RELEASE(_physics);
	PX_RELEASE(_foundation);
	SY_LOG_PHYS(SY_LOGLEVEL_INFO, "Physics system destruction successful. ");
	return SyResult();
}

SyResult SyRigidBodySystem::InitComponent(const entt::entity& entity, SyRBodyComponent& rbComponent, TransformComponent& tComponent)
{
	SyResult result;
	switch (rbComponent._rbType)
	{
	case SY_RB_TYPE_STATIC: rbComponent._rbActor = _physics->createRigidStatic(PxTransform(rbComponent._origin, 
														SyVector3::EulerToPxQuat(rbComponent._rotation)));
		break;
	case SY_RB_TYPE_DYNAMIC: rbComponent._rbActor = _physics->createRigidDynamic(PxTransform(rbComponent._origin, 
														SyVector3::EulerToPxQuat(rbComponent._rotation)));
		break;
	default:
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Unknown Rigid Body type in SyRbComponent. Entity: %d.", (unsigned)entity);
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("Unknown Rigid Body type in SyRbComponent. Entity: %d.", (unsigned)entity);
		return result;
	};
	// switch (rbComponent._rbShapeType)
	// {
	// case SY_RB_SHAPE_TYPE_BOX:
	// {
	// 	auto boxDesc = static_cast<const SyRBodyBoxShapeDesc&>(rbComponent._rbShapeDesc);
	// 	PxRigidActorExt::createExclusiveShape(*(rbComponent._rbActor),
	// 		PxBoxGeometry(boxDesc.HalfExtent),
	// 		*physics->createMaterial(
	// 			rbComponent._rbMaterial.staticFriction,
	// 			rbComponent._rbMaterial.dynamicFriction,
	// 			rbComponent._rbMaterial.restitution));
	// }
	// break;
	// case SY_RB_SHAPE_TYPE_SPHERE:
	// 	auto sphereDesc = static_cast<const SyRBodySphereShapeDesc&>(rbComponent._rbShapeDesc);
	// 	PxRigidActorExt::createExclusiveShape(*(rbComponent._rbActor),
	// 		PxSphereGeometry(sphereDesc.Radius),
	// 		*physics->createMaterial(
	// 			rbComponent._rbMaterial.staticFriction,
	// 			rbComponent._rbMaterial.dynamicFriction,
	// 			rbComponent._rbMaterial.restitution));
	// break;
	// case SY_RB_SHAPE_TYPE_TRIMESH:
	// 		auto trimeshDesc = static_cast<const SyRBodyTrimeshShapeDesc&>(rbComponent._rbShapeDesc);
	// 		PxTriangleMeshDesc meshDesc;
	// 		meshDesc.points.count		= trimeshDesc.NumVertices;
	// 		meshDesc.points.data		= trimeshDesc.FirstVertex;
	// 		meshDesc.points.stride		= trimeshDesc.VERTEX_STRIDE;
	// 		meshDesc.triangles.count	= trimeshDesc.NumTriangles;
	// 		meshDesc.triangles.data		= trimeshDesc.FirstIndex;
	// 		meshDesc.triangles.stride	= trimeshDesc.TRIANGLE_STRIDE;
	// 		PxTolerancesScale scale;
	// 		PxCookingParams params(scale);
	// 		PxDefaultMemoryOutputStream writeBuffer;
	// 		PxTriangleMeshCookingResult::Enum cookingResult;
	// 		bool trimeshCookingStatus = PxCookTriangleMesh(params, meshDesc, writeBuffer, &cookingResult);
	// 		if (trimeshCookingStatus != true)
	// 		{
	// 			SY_LOGLEVEL_ERROR("Failed to cook triangle mesh in SyRbComponent. Entity: %d.", (unsigned)entity);
	// 			result.code = SY_RESCODE_ERROR;
	// 			result.message = xstring("Failed to cook triangle mesh in SyRbComponent. Entity: %d.", (unsigned)entity);
	// 			return result;
	// 		}
	// 		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	// 		auto* triMesh = physics->createTriangleMesh(readBuffer);
	// 	break;
	// default:
	// 	SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Unknown rigid body shape type.");
	// 	
	// 	return result;
	// }
	if (rbComponent._rbType == SY_RB_TYPE_DYNAMIC)
	{
		bool updateMassResult = PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidBody*>(rbComponent._rbActor), rbComponent._rbMaterial.density);
		if (updateMassResult == false)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidBodyExt::updateMassAndInertia returned false");
			return result;
		}
	}
	_scene->addActor(*rbComponent._rbActor);
}
