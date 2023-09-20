#include "RBodyComponent.h"
#include "PxConfig.h"
#include "PxPhysicsAPI.h"
#include "../Tools/ErrorLogger.h"
#include <iostream>
using namespace physx;

SyRBodyComponent::SyRBodyComponent(	const SyRBodyType&			rbType_, 
									const SyRBodyShapeType&		rbShapeType_,
									const SyRBodyShapeDescBase& rbDefaultShapeDesc_,
									const SyRBodyMaterial&		rbMaterial_)
{	rbType = rbType_;
	rbShapeType = rbShapeType_;
	rbMaterial = rbMaterial_;
	if (physics == nullptr)
		//SyRbodyComponent::physics has not been initialized
		ErrorLogger::Log(SY_GENERIC_ERROR_CRITICAL, "RBodyComponent.cpp", 16);
	if (scene == nullptr)
		//SyRbodyComponent::scene has not been initialized
		ErrorLogger::Log(SY_GENERIC_ERROR_CRITICAL, "RBodyComponent.cpp", 16);

	switch (rbType)
	{
	case RB_TYPE_STATIC: rbActor = physics->createRigidStatic(PxTransform(rbDefaultShapeDesc->origin));
		break;
	case RB_TYPE_DYNAMIC: rbActor = physics->createRigidDynamic(PxTransform(rbDefaultShapeDesc->origin));
		break;
	default:
		//Unknown RB_TYPE!;
		ErrorLogger::Log(SY_GENERIC_ERROR_CRITICAL, "RBodyComponent.cpp", 40);
		break;
	};
	switch (rbShapeType)
	{
	case RB_SHAPE_TYPE_BOX:
	{
		auto boxDesc = static_cast<const SyRBodyBoxShapeDesc&>(rbDefaultShapeDesc_);
		PxRigidActorExt::createExclusiveShape(*rbActor,
			PxBoxGeometry(boxDesc.halfExt),
			*physics->createMaterial(
				rbMaterial.staticFriction,
				rbMaterial.dynamicFriction,
				rbMaterial.restitution));
	}
	break;
	case RB_SHAPE_TYPE_SPHERE:
	{
		auto sphereDesc = static_cast<const SyRBodySphereShapeDesc&>(rbDefaultShapeDesc_);
		PxRigidActorExt::createExclusiveShape(*rbActor,
			PxSphereGeometry(sphereDesc.radius),
			*physics->createMaterial(
				rbMaterial.staticFriction,
				rbMaterial.dynamicFriction,
				rbMaterial.restitution));
	}
	break;
	default:
		//Unknown RB_SHAPE_TYPE!
		ErrorLogger::Log(SY_GENERIC_ERROR_CRITICAL, "RBodyComponent.cpp", 70);
		break;
	}
	scene->addActor(*rbActor);
}


SyRBodyComponent::~SyRBodyComponent()
{
	delete rbDefaultShapeDesc;
	PX_RELEASE(rbActor);
	PX_RELEASE(rbShape);
}

