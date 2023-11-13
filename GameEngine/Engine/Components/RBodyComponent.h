#pragma once
#include <PxMaterial.h>

#include "../Tools/Data/Vector.h"
#include "../Tools/ErrorLogger.h"
#include "../Tools/Data/FlagBitmask.h"
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
	USE_DENSITY		 = 1,
	KINEMATIC		= 1 << 1
};
DEFINE_BITWISE_OPERATORS(SyERBodyFlags);

struct SyRBodyComponent
{
	SyRBodyComponent(	const SyERBodyType&	rbType,
						float 				mass = 1.0f,
						unsigned			flags = 0);
	
	~SyRBodyComponent();
private:
	//public fields initialized in constructor

	SyERBodyType		_rbType;
	float				_mass;
	unsigned			_flags;
	
	//public fields
	
	SyVector3			_linearVelocity = SyVector3::ZERO;
	SyVector3			_angularVelocity = SyVector3::ZERO;
	
	//fields initialized in SyPhysicsSystem::Init
	
	physx::PxRigidActor*	_rbActor = nullptr;

	static physx::PxPhysics*	_physics;
	static physx::PxScene*		_scene;
	
	friend class SyCollisionSystem;
	friend class SyRBodySystem;
};

struct SyRbCreateOnNextUpdateTag
{
	
};

