#pragma once
#include "../Tools/Data/Vector.h"
#pragma region forward declaration
namespace physx
{
	class PxShape;
	class PxRigidActor;
	class PxPhysics;
	class PxScene;

}
#pragma endregion

struct SyRBodyMaterial
{
	float staticFriction;
	float dynamicFriction;
	float restitution;
};
enum SyRBodyType
{
	RB_TYPE_STATIC = 0,
	RB_TYPE_DYNAMIC = 1
};
enum SyRBodyShapeType
{
	RB_SHAPE_TYPE_BOX = 0,
	RB_SHAPE_TYPE_SPHERE = 1
};

struct SyRBodyShapeDescBase
{
	SyVector3 origin;
};

struct SyRBodyBoxShapeDesc : SyRBodyShapeDescBase
{
	SyVector3 halfExt;
};
struct SyRBodySphereShapeDesc : SyRBodyShapeDescBase
{
	float radius;
};
struct SyRBodyComponent
{
	SyRBodyComponent(	const SyRBodyType&			rbType_, 
						const SyRBodyShapeType&		rbShapeType_,
						const SyRBodyShapeDescBase& rbDefaultShapeDesc_,
						const SyRBodyMaterial&		rbMaterial_);
	
	~SyRBodyComponent();
private:
	SyRBodyType					rbType;
	physx::PxRigidActor*		rbActor = nullptr;
	SyRBodyShapeType			rbShapeType;
	physx::PxShape*				rbShape = nullptr;
	SyRBodyMaterial				rbMaterial;
	SyRBodyShapeDescBase*		rbDefaultShapeDesc =  nullptr;
	static physx::PxPhysics*	physics;
	static physx::PxScene*		scene;

	friend class SyPhysicsSystem;
};

