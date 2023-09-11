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
		resultMat =resultMat* (curTc.rotationMatrix*curTc.translationMatrix);
		curID = curTc.parent;
	}
	return resultMat;
}

Matrix TransformHelper::ConstructLocalTransformMatrix(TransformComponent& tc)
{
	return  tc.scaleMatrix * tc.rotationMatrix * tc.translationMatrix;
}


Matrix TransformHelper::ConstructInverseParentTransform(TransformComponent& tc)
{
	Matrix resultMat = Matrix::Identity;
	entt::entity curID = tc.parent;

	while (curID != entt::null) {
		TransformComponent& curTc = GetScene()->registry.get<TransformComponent>(curID);
		resultMat = resultMat * (curTc.rotationMatrix * curTc.translationMatrix);
		curID = curTc.parent;
	}
	return resultMat.Invert();
}



Vector3 TransformHelper::GetRotationDegrees(TransformComponent& tc)
{
	Vector3 rot = tc.localRotation;
	RadToDegrees(rot);
	return rot;
}

void TransformHelper::UpdateRelativeToParent(const TransformComponent* parent, TransformComponent& tc)
{
	if (!parent)
	{
		auto mat = ConstructLocalTransformMatrix(tc);
		auto matInverse = ConstructInverseParentTransform(tc);
		matInverse = matInverse.Invert();
		mat = mat*matInverse;
		Vector3 translation, scale;
		Quaternion rotation;
		mat.Decompose(scale, rotation, translation);
		tc.localPosition = translation;
		tc.localRotation = rotation.ToEuler();
		tc.localScale = scale;

		tc.scaleMatrix = Matrix::CreateScale(scale);
		tc.rotationMatrix = Matrix::CreateFromYawPitchRoll(tc.localRotation);
		tc.translationMatrix = Matrix::CreateTranslation(translation);
	}
	else
	{
		auto mat=ConstructLocalTransformMatrix(tc);
		auto matInverse = ConstructInverseParentTransform(tc);
		mat = mat*matInverse;
		Vector3 translation, scale;
		Quaternion rotation;
		mat.Decompose(scale, rotation, translation);
		tc.localPosition = translation;
		tc.localRotation = rotation.ToEuler();
		tc.localScale = scale;

		tc.scaleMatrix = Matrix::CreateScale(scale);
		tc.rotationMatrix = Matrix::CreateFromYawPitchRoll(tc.localRotation);
		tc.translationMatrix = Matrix::CreateTranslation(translation);
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
