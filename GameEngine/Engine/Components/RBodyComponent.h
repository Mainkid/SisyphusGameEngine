#pragma once
#include <PxMaterial.h>

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
						float						mass			= 1.0f);
	
	~SyRBodyComponent();
private:
	//fields initialized in constructor

	SyRBodyType			_rbType;
	float				_mass;
	SyVector3			_origin;
	SyVector3			_rotation;
	
	//fields initialized in SyPhysicsSystem::Init

	physx::PxRigidActor*	_rbActor = nullptr;
	physx::PxShape*			_rbShape = nullptr;
	std::size_t hash;

	static physx::PxPhysics* _physics;
	static physx::PxScene* _scene;
	
	friend class SyCollisionSystem;
	friend class SyRBodySystem;
};

struct SyRbCreateOnNextUpdateTag
{
	
};

