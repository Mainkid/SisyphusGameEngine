#include "RBodyComponent.h"
#include "PxConfig.h"
#include "PxPhysicsAPI.h"
#include <iostream>
using namespace physx;

SyRBodyComponent::SyRBodyComponent(	PxPhysics*					psPhysics_, 
									PxScene*					psScene_, 
									const SyRBodyType&			rbType_, 
									const SyRBodyShapeType&		rbShapeType_,
									const SyRBodyShapeDescBase&	rbShapeDesc_,
									const SyRBodyMaterial&		rbMaterial_)
{
	physics = psPhysics_;
	scene = psScene_;
	rbDefaultShapeDesc = new SyRBodyShapeDescBase(rbShapeDesc_);
	rbType = rbType_;
	rbShapeType = rbShapeType_;
	rbMaterial = rbMaterial_;
	
}

void SyRBodyComponent::Init()
{
	
	switch (rbType)
	{
	case RB_TYPE_STATIC: rbActor = physics->createRigidStatic(PxTransform(rbDefaultShapeDesc->origin));
		break;
	case RB_TYPE_DYNAMIC: rbActor = physics->createRigidDynamic(PxTransform(rbDefaultShapeDesc->origin));
		break;
	default:
		std::cout << "Unknown RB_TYPE!\n";
		return;
		break;
	};
	switch (rbShapeType)
	{
	case RB_SHAPE_TYPE_BOX:
	{
		auto boxDesc = static_cast<SyRBodyBoxShapeDesc*>(rbDefaultShapeDesc);
		PxRigidActorExt::createExclusiveShape(*rbActor,
			PxBoxGeometry(boxDesc->halfExt),
			*physics->createMaterial(
				rbMaterial.staticFriction,
				rbMaterial.dynamicFriction,
				rbMaterial.restitution));
	}
	break;
	case RB_SHAPE_TYPE_SPHERE:
	{
		auto sphereDesc = static_cast<SyRBodySphereShapeDesc*>(rbDefaultShapeDesc);
		PxRigidActorExt::createExclusiveShape(*rbActor,
			PxSphereGeometry(sphereDesc->radius),
			*physics->createMaterial(
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
	scene->addActor(*rbActor);

}

SyRBodyComponent::~SyRBodyComponent()
{
	delete rbDefaultShapeDesc;
	PX_RELEASE(rbActor);
	PX_RELEASE(rbShape);
}

