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
	float staticFriction =	1.0f;
	float dynamicFriction = 1.0f;
	float restitution =		0.3f;
	float density =			0.0001f;
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
	SyVector3 origin = {0.0f, 0.0f, 0.0f};
	SyVector3 rotation = { 0.0f, 0.0f, 0.0f };
};

struct SyRBodyBoxShapeDesc : SyRBodyShapeDescBase
{
	SyVector3 halfExt = {1.0f, 1.0f, 1.0f};
};
struct SyRBodySphereShapeDesc : SyRBodyShapeDescBase
{
	float radius = 1.0f;
};
struct SyRBodyComponent
{
	SyRBodyComponent(	const SyRBodyType&			rbType_ = RB_TYPE_DYNAMIC,
						const SyRBodyShapeType&		rbShapeType_ = RB_SHAPE_TYPE_BOX,
						const SyRBodyShapeDescBase& rbDefaultShapeDesc_ = SyRBodyBoxShapeDesc(),
						const SyRBodyMaterial&		rbMaterial_ = SyRBodyMaterial());
	
	~SyRBodyComponent();
private:
	SyRBodyType					rbType;
	physx::PxRigidActor*		rbActor = nullptr;
	SyRBodyShapeType			rbShapeType;
	physx::PxShape*				rbShape = nullptr;
	SyRBodyMaterial				rbMaterial;
	static physx::PxPhysics*	physics;
	static physx::PxScene*		scene;

	friend class SyPhysicsSystem;
};

