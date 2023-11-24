#pragma once
#include <PxMaterial.h>

#include "../../../Core/Tools/Structures/Vector.h"
#include "../../../Core/Tools/ErrorLogger.h"
#include "../../../Core/Tools/FlagBitmask.h"
#include "../../vendor/entt/entt.hpp"
#pragma region forward declaration
namespace physx
{
	class PxShape;
	class PxRigidActor;
	class PxPhysics;
	class PxScene;

}
#pragma endregion

enum SyERBodyType
{
	STATIC	= 0,
	DYNAMIC	= 1
};

struct SyRbTransform
{
	SyVector3 Origin				= {0.0f, 0.0f, 0.0f};
	SyVector3 Rotation				= {0.0f, 0.0f, 0.0f};
};

enum SyERBodyFlags
{
	USE_DENSITY		= 1,
	KINEMATIC		= 1 << 1,
	DISABLE_GRAVITY	= 1 << 2
};
DEFINE_BITWISE_OPERATORS(SyERBodyFlags);

struct SyRBodyComponent
{
	SyRBodyComponent(	const SyERBodyType&	rbType = SyERBodyType::STATIC,
						float 				mass = 1.0f,
						unsigned			flags = 0);
	
	~SyRBodyComponent();

	//public fields initialized in constructor and can be modified during runtime
	SyERBodyType		RbType; //runtime modification is not supported, but will be added in future. The member has to be added to properties
	float				Mass;
	unsigned			Flags;
	
	//public fields, that can be modified during runtime
	
	SyVector3			LinearVelocity = SyVector3::ZERO;
	SyVector3			AngularVelocity = SyVector3::ZERO;

private:
	//members that are used by developers only
	
	
	//private fields initialized in SyPhysicsSystem::Init
	
	physx::PxRigidActor*	_rbActor = nullptr;

	static physx::PxPhysics*	_physics;
	static physx::PxScene*		_scene;
	
	friend class SyCollisionSystem;
	friend class SyCollisionPreSystem;
	friend class SyRBodySystem;	
};

