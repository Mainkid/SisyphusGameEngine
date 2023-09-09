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

Matrix TransformHelper::ConstructLocalTransformMatrix(TransformComponent& tc)
{
	return  tc.scaleMatrix * tc.rotationMatrix * tc.translationMatrix;
}

Matrix TransformHelper::ConstructWorldTransformMatrix(TransformComponent& tc)
{
	return  Matrix::CreateScale(tc.scale) * Matrix::CreateFromYawPitchRoll(tc.rotation) * Matrix::CreateTranslation(tc.position);
}

void TransformHelper::RecalculateWorldPos(TransformComponent& tc)
{
	entt::entity curID = tc.parent;
	tc.rotation = tc.localRotation;
	tc.position = tc.localPosition;
	while (curID != entt::null) {
		TransformComponent& curTc = GetScene()->registry.get<TransformComponent>(curID);
		tc.rotation += curTc.localRotation;
		tc.position += curTc.localPosition;
		curID = curTc.parent;
	}
}

Vector3 TransformHelper::GetRotationDegrees(TransformComponent& tc)
{
	Vector3 rot = tc.localRotation;
	RadToDegrees(rot);
	return rot;
}

Vector3 TransformHelper::GetRealScale(TransformComponent& tc, Vector3 scale)
{
	return Vector3::Zero;
	// TODO: вставьте здесь оператор return
}

Vector3 TransformHelper::GetRealTranslation(TransformComponent& tc, Vector3 transform)
{
	entt::entity curID = tc.parent;
	
	while (curID != entt::null) {
		TransformComponent& curTc = GetScene()->registry.get<TransformComponent>(curID);
		transform -= curTc.localPosition;
		curID = curTc.parent;
	}
	return transform;
}

Vector3 TransformHelper::GetRealRotation(TransformComponent& tc, Vector3 rotation)
{
	entt::entity curID = tc.parent;

	while (curID != entt::null) {
		TransformComponent& curTc = GetScene()->registry.get<TransformComponent>(curID);
		rotation -= curTc.localRotation;
		curID = curTc.parent;
	}
	return rotation;
}

void TransformHelper::UpdateRelativeToParent(const TransformComponent* parent, TransformComponent& tc)
{
	if (!parent)
	{
		entt::entity curID = tc.parent;
		while (curID != entt::null)
		{
			TransformComponent& curTc =GetScene()->registry.get<TransformComponent>(curID);
			tc.localPosition += curTc.localPosition;
			tc.localRotation += curTc.localRotation;
			curID = curTc.parent;
		}
	}
	else
	{
		entt::entity curID = tc.parent;
		tc.rotation = tc.localRotation;
		tc.position = tc.localPosition;
		while (curID != entt::null)
		{
			TransformComponent& curTc = GetScene()->registry.get<TransformComponent>(curID);
			tc.localPosition -= curTc.localPosition;
			tc.localRotation -= curTc.localRotation;
			tc.rotation += curTc.localRotation;
			tc.position += curTc.localPosition;
			curID = curTc.parent;
		}
	}
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
