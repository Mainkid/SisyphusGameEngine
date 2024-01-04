#pragma once
#include "SimpleMath.h"
#include "../Components/TransformComponent.h"
#include "boost/container_hash/hash.hpp"

class TransformHelper
{
public:
	static Matrix ConstructLocalTransformMatrix(TransformComponent& tc);
	static Matrix ConstructInverseParentTransform(TransformComponent& tc);
	static void UpdateTransformMatrix(TransformComponent& tc);
	static void UpdateWorldTransformMatrix(TransformComponent& tc,Matrix parentTransform);
	static void UpdateRelativeToParent(const TransformComponent* parent, TransformComponent& tc);
	static bool HasHierarchyCycles(entt::registry* ecs, entt::entity sourceGameObject, entt::entity parentGameObject);
	static Vector3 GetRotationDegrees(TransformComponent& tc);
	static void DegreesToRad(Vector3& vec);
	static void RadToDegrees(Vector3& vec);
	static SyVector3 DegreesToRad(const SyVector3& vec);
	static SyVector3 RadToDegrees(const SyVector3& vec);
	static void SetDefaultHash();

	inline static size_t worldHashDefault;
	inline static size_t localHashDefault;
};

