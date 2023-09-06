#pragma once
#include "SimpleMath.h"
#include "../Components/TransformComponent.h"

class TransformHelper
{
public:
	static Matrix ConstructTransformMatrix(TransformComponent& tc);
	static Vector3 GetRotationDegrees(TransformComponent& tc);
	static void DegreesToRad(Vector3& vec);
	static void RadToDegrees(Vector3& vec);
};

