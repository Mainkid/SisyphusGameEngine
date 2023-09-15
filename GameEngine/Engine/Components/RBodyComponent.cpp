#include "RBodyComponent.h"
#include "../../vendor/PhysX/PxPhysicsAPI.h"
#include <iostream>
using namespace physx;

inline PxVec3 FromDxVector3(const DirectX::SimpleMath::Vector3& dxVector3)
{
	return PxVec3(
		dxVector3.x,
		dxVector3.y,
		dxVector3.z
	);
}
//must create separate class for conversions!!!
	
RBodyComponent::RBodyComponent(	PxPhysics&					psPhysics_, 
								PxScene&					psScene_, 
								const RBodyType&			rbType_, 
								const RBodyShapeType&		rbShapeType_,
								const RBodyShapeDescBase&	rbShapeDesc_,
								const RBodyMaterial&		rbMaterial_)
{
	rbType = rbType_;
	switch (rbType)
	{
	case RB_TYPE_STATIC: rbActor = psPhysics_.createRigidStatic(PxTransform(
					FromDxVector3(rbShapeDesc_.origin)));
	break;
	case RB_TYPE_DYNAMIC: rbActor = psPhysics_.createRigidDynamic(PxTransform(
					FromDxVector3(rbShapeDesc_.origin)));
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
		const RBodyBoxShapeDesc& boxDesc = static_cast<const RBodyBoxShapeDesc&>(rbShapeDesc_);
		PxRigidActorExt::createExclusiveShape(	*rbActor,
												PxBoxGeometry(FromDxVector3(boxDesc.halfExt)),
												*psPhysics_.createMaterial(
												rbMaterial.staticFriction,
												rbMaterial.dynamicFriction,
												rbMaterial.restitution));
	}
	break;
	case RB_SHAPE_TYPE_SPHERE:
	{
		const RBodySphereShapeDesc& sphereDesc = static_cast<const RBodySphereShapeDesc&>(rbShapeDesc_);
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

RBodyComponent::~RBodyComponent()
{
	PX_RELEASE(rbActor);
	PX_RELEASE(rbShape);
}

