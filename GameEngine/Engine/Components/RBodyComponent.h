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

enum SyRBodyType
{
	SY_RB_TYPE_STATIC	= 0,
	SY_RB_TYPE_DYNAMIC	= 1
};

struct SyRbTransform
{
	SyVector3 Origin				= {0.0f, 0.0f, 0.0f};
	SyVector3 Rotation				= {0.0f, 0.0f, 0.0f};
};

enum SyRBodyFlags
{
	SY_RB_USE_DENSITY		 = 1,
	SY_RB_IS_KINEMATIC		= 1 << 1
};
DEFINE_BITWISE_OPERATORS(SyRBodyFlags);

struct SyRBodyComponent
{
	SyRBodyComponent(	const SyRBodyType&	rbType,
						float 				mass = 1.0f,
						unsigned			flags = 0);
	
	~SyRBodyComponent();
private:
	//fields initialized in constructor

	SyRBodyType			_rbType;
	float				_mass;
	unsigned			_flags;
	//fields initialized in SyPhysicsSystem::Init
	
	physx::PxRigidActor*	_rbActor = nullptr;
	std::size_t hash;

	static physx::PxPhysics*	_physics;
	static physx::PxScene*		_scene;
	
	friend class SyCollisionSystem;
	friend class SyRBodySystem;
};

struct SyRbCreateOnNextUpdateTag
{
	
};

