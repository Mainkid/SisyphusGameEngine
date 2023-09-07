#include "TransformHelper.h"
#include "../Core/EngineCore.h"
#define _USE_MATH_DEFINES
#include <math.h>

Matrix TransformHelper::ConstructTransformMatrix(TransformComponent& tc)
{
	Matrix resultMat = tc.scaleMatrix * tc.rotationMatrix * tc.translationMatrix;
	entt::entity curID = tc.parent;

	while (curID != entt::null) {
		TransformComponent& curTc = GetScene()->registry.get<TransformComponent>(curID);
		resultMat *= curTc.rotationMatrix * curTc.translationMatrix;
		curID = curTc.parent;
	}
	return resultMat;
}

Vector3 TransformHelper::GetRotationDegrees(TransformComponent& tc)
{
	Vector3 rot = tc.rotation;
	RadToDegrees(rot);
	return rot;
}

void TransformHelper::DegreesToRad(Vector3& vec)
{
	vec.x *= M_PI / 180.0f;
	vec.y *= M_PI / 180.0f;
	vec.z *= M_PI / 180.0f;
}

void TransformHelper::RadToDegrees(Vector3& vec)
{
	vec.x *= 180.0f / M_PI;
	vec.y *= 180.0f / M_PI;
	vec.z *= 180.0f / M_PI;
}
