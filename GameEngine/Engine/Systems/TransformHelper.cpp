#include "TransformHelper.h"
#include "../Core/EngineCore.h"
#include "../Core/ServiceLocator.h"
#include "../Systems/EngineContext.h"
#define _USE_MATH_DEFINES
#include <math.h>

Matrix TransformHelper::ConstructLocalTransformMatrix(TransformComponent& tc)
{
	return  (Matrix::CreateScale(tc.localScale) * Matrix::CreateFromYawPitchRoll(tc.localRotation) * Matrix::CreateTranslation(tc.localPosition));
}


Matrix TransformHelper::ConstructInverseParentTransform(TransformComponent& tc)
{
	Matrix resultMat = Matrix::Identity;
	EngineContext* ec = ServiceLocator::instance()->Get<EngineContext>();
	entt::entity curID = tc.parent;

	while (curID != entt::null) {
		TransformComponent& curTc = ec->scene->registry.get<TransformComponent>(curID);
		resultMat = resultMat * (Matrix::CreateScale(curTc.localScale)* Matrix::CreateFromYawPitchRoll(curTc.localRotation) * Matrix::CreateTranslation(curTc.localPosition));
		curID = curTc.parent;
	}
	return resultMat.Invert();
}

void TransformHelper::UpdateTransformMatrix(TransformComponent& tc)
{
	EngineContext* ec = ServiceLocator::instance()->Get<EngineContext>();
	tc.transformMatrix = Matrix::CreateScale(tc.localScale) * Matrix::CreateFromYawPitchRoll(tc.localRotation) * Matrix::CreateTranslation(tc.localPosition);
	entt::entity curID = tc.parent;
	if (curID!=entt::null)
	{
		TransformComponent& curTc = ec->scene->registry.get<TransformComponent>(curID);
		tc.transformMatrix = tc.transformMatrix * curTc.transformMatrix;
	}

	for (auto& child : tc.children)
	{
		UpdateTransformMatrix(ec->scene->registry.get<TransformComponent>(child));
	}
	Quaternion q;
	Vector3 scaleNew = tc.scale;
	Vector3 positionNew = tc.position;
	tc.transformMatrix.Decompose(scaleNew, q, positionNew);
	tc.position = positionNew;
	tc.scale = scaleNew;
	tc.rotation = q.ToEuler();
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
		tc.transformMatrix = (Matrix::CreateScale(scale) * Matrix::CreateFromYawPitchRoll(tc.localRotation) * Matrix::CreateTranslation(translation));
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
		tc.transformMatrix = (Matrix::CreateScale(scale) * Matrix::CreateFromYawPitchRoll(tc.localRotation) * Matrix::CreateTranslation(translation));
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
