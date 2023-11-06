#pragma once
#include <boost/function_types/property_tags.hpp>

#include "../Tools/Data/Vector.h"
#include "../Tools/ErrorLogger.h"
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
	float staticFriction	= 1.0f;
	float dynamicFriction	= 1.0f;
	float restitution		= 0.3f;
	float density			= 0.0001f;
};
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

struct SyRBodyComponent
{
	SyRBodyComponent(	const SyRBodyType&			rbType,
						const SyRbTransform&		rbTransform,
						const SyRBodyMaterial&		rbMaterial		= SyRBodyMaterial(),
						bool						manuallySetMass = true,
						float						mass			= 1.0f);
	
	~SyRBodyComponent();
private:
	//fields initialized in constructor

	SyRBodyType			_rbType;
	SyRBodyMaterial		_rbMaterial;
	bool				_manuallySetMass;
	float				_mass;
	SyVector3			_origin;
	SyVector3			_rotation;
	//fields initialized in SyPhysicsSystem::Init

	//actor, containing collider shape of the rigid body, sphere by default
	physx::PxRigidActor*	_rbActor = nullptr;
	physx::PxShape*			_rbShape = nullptr;
	std::size_t hash;
	
	static	physx::PxPhysics*	_physics;
	static	physx::PxScene*		_scene;

	friend class SyPrimitiveColliderComponent;
	friend class SyRigidBodySystem;
};

struct SyRbCreateOnNextUpdateTag
{
	
};

