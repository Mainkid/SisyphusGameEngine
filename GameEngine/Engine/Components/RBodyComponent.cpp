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
	{
		SY_LOG_PHYS(SY_LOGLEVEL_CRITICAL, L"SyRbodyComponent::physics == nullptr");
		exit(-1);
	}
	if (scene == nullptr)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_CRITICAL, L"SyRbodyComponent::scene == nullptr");
		exit(-1);
	}

	switch (rbType)
	{
	case RB_TYPE_STATIC: rbActor = physics->createRigidStatic(PxTransform(rbDefaultShapeDesc_.origin, 
														SyVector3::EulerToPxQuat(rbDefaultShapeDesc_.rotation)));
		break;
	case RB_TYPE_DYNAMIC: rbActor = physics->createRigidDynamic(PxTransform(rbDefaultShapeDesc_.origin, 
														SyVector3::EulerToPxQuat(rbDefaultShapeDesc_.rotation)));
		break;
	default:
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, L"Unknown rigid body type.");
		return;
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
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, L"Unknown rigid body shape type.");
		return;
	}
	if (rbType == RB_TYPE_DYNAMIC)
	{
		bool updateMassResult = PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidBody*>(rbActor), rbMaterial.density);
		if (updateMassResult == false)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, L"PxRigidBodyExt::updateMassAndInertia returned false");
		}
	}
	scene->addActor(*rbActor);
}


SyRBodyComponent::~SyRBodyComponent()
{
	PX_RELEASE(rbActor);
	PX_RELEASE(rbShape);
}

