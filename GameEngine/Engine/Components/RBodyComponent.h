#pragma once
#include "SimpleMath.h"
#pragma region forward declaration
namespace physx
{
	class PxShape;
	class PxRigidActor;
	class PxPhysics;
	class PxScene;

}
#pragma endregion

struct RBodyMaterial
{
	float staticFriction;
	float dynamicFriction;
	float restitution;
};
enum RBodyType
{
	RB_TYPE_STATIC = 0,
	RB_TYPE_DYNAMIC = 1
};
enum RBodyShapeType
{
	RB_SHAPE_TYPE_BOX = 0,
	RB_SHAPE_TYPE_SPHERE = 1
};

struct RBodyShapeDescBase
{
	DirectX::SimpleMath::Vector3 origin;
};

struct RBodyBoxShapeDesc : RBodyShapeDescBase
{
	DirectX::SimpleMath::Vector3 halfExt;
};
struct RBodySphereShapeDesc : RBodyShapeDescBase
{
	float radius;
};
struct RBodyComponent
{

	RBodyType				rbType;
	physx::PxRigidActor*	rbActor = nullptr;
	RBodyShapeType			rbShapeType;
	physx::PxShape*			rbShape = nullptr;
	RBodyMaterial			rbMaterial;

	RBodyComponent(	physx::PxPhysics&		psPhysics_, 
					physx::PxScene&			psScene_, 
					const RBodyType&			rbType_, 
					const RBodyShapeType&		rbShapeType_, 
					const RBodyShapeDescBase&	rbShapeDesc_, 
					const RBodyMaterial&		rbMaterial_);
	~RBodyComponent();
};

