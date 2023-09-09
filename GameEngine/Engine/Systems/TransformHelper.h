#pragma once
#include "SimpleMath.h"
#include "../Components/TransformComponent.h"

class TransformHelper
{
public:
	static Matrix ConstructTransformMatrix(TransformComponent& tc);
	static Matrix ConstructLocalTransformMatrix(TransformComponent& tc);
	static Matrix ConstructWorldTransformMatrix(TransformComponent& tc);
	static void RecalculateWorldPos(TransformComponent& tc);
	static Vector3 GetRotationDegrees(TransformComponent& tc);
	static Vector3 GetRealScale(TransformComponent& tc, Vector3 scale);
	static Vector3 GetRealTranslation(TransformComponent& tc, Vector3 transform);
	static Vector3 GetRealRotation(TransformComponent& tc, Vector3 rotation);
	static void UpdateRelativeToParent(const TransformComponent* parent, TransformComponent& tc);
	static void DegreesToRad(Vector3& vec);
	static void RadToDegrees(Vector3& vec);
};

