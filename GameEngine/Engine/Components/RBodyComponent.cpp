#include "RBodyComponent.h"
#include "PxConfig.h"
#include "PxPhysicsAPI.h"
#include <iostream>
using namespace physx;

SyRBodyComponent::SyRBodyComponent(	PxPhysics&					psPhysics_, 
									PxScene&					psScene_, 
									const SyRBodyType&			rbType_, 
									const SyRBodyShapeType&		rbShapeType_,
									const SyRBodyShapeDescBase&	rbShapeDesc_,
									const SyRBodyMaterial&		rbMaterial_)
{
	rbType = rbType_;
	switch (rbType)
	{
	case RB_TYPE_STATIC: rbActor = psPhysics_.createRigidStatic(PxTransform(rbShapeDesc_.origin));
	break;
	case RB_TYPE_DYNAMIC: rbActor = psPhysics_.createRigidDynamic(PxTransform(rbShapeDesc_.origin));
	break;
	default:
		std::cout << "Unknown RB_TYPE!\n";
		return;
	break;
	}
	rbShapeType = rbShapeType_;
	rbMaterial = rbMaterial_;
	PxMaterial* psMaterial = psPhysics_.createMaterial(
											rbMaterial.staticFriction, 
											rbMaterial.dynamicFriction, 
											rbMaterial.restitution);
	switch (rbShapeType)
	{
	case RB_SHAPE_TYPE_BOX: 
	{
		const SyRBodyBoxShapeDesc& boxDesc = static_cast<const SyRBodyBoxShapeDesc&>(rbShapeDesc_);
		PxRigidActorExt::createExclusiveShape(	*rbActor,
												PxBoxGeometry(boxDesc.halfExt),
												*psPhysics_.createMaterial(
												rbMaterial.staticFriction,
												rbMaterial.dynamicFriction,
												rbMaterial.restitution));
	}
	break;
	case RB_SHAPE_TYPE_SPHERE:
	{
		const SyRBodySphereShapeDesc& sphereDesc = static_cast<const SyRBodySphereShapeDesc&>(rbShapeDesc_);
		PxRigidActorExt::createExclusiveShape(	*rbActor,
												PxSphereGeometry(sphereDesc.radius),
												*psPhysics_.createMaterial(
												rbMaterial.staticFriction,
												rbMaterial.dynamicFriction,
												rbMaterial.restitution));
	}
	break;
	default:
		std::cout << "Unknown RB_SHAPE_TYPE!\n";
		return;
	break;
	}
	psScene_.addActor(*rbActor);
}

SyRBodyComponent::~SyRBodyComponent()
{
	PX_RELEASE(rbActor);
	PX_RELEASE(rbShape);
}

