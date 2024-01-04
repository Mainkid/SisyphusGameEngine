#include "TransformHelper.h"
#include "../Core/EngineCore.h"
#include "../Core/ServiceLocator.h"
#include "../Contexts/EngineContext.h"
#include "../Core/Tools/Structures/Vector.h"

#define _USE_MATH_DEFINES
#include <math.h>

Matrix TransformHelper::ConstructLocalTransformMatrix(TransformComponent& tc)
{
	return  (Matrix::CreateScale(tc.localScale) * Matrix::CreateFromYawPitchRoll(tc.localRotation) * Matrix::CreateTranslation(tc.localPosition));
}


Matrix TransformHelper::ConstructInverseParentTransform(TransformComponent& tc)
{
	Matrix resultMat = Matrix::Identity;
	EngineContext* context = ServiceLocator::instance()->Get<EngineContext>();
	entt::entity curID = static_cast<entt::entity>(tc.parent);

	while (curID != entt::null) 
	{
		TransformComponent& curTc = context->ecs.get<TransformComponent>(curID);
		resultMat = resultMat * (Matrix::CreateScale(curTc.localScale)* Matrix::CreateFromYawPitchRoll(curTc.localRotation) * Matrix::CreateTranslation(curTc.localPosition));
		curID = static_cast<entt::entity>(curTc.parent);
	}
	return resultMat.Invert();
}

void TransformHelper::UpdateTransformMatrix(TransformComponent& tc)
{
	EngineContext* context = ServiceLocator::instance()->Get<EngineContext>();
	tc.transformMatrix = Matrix::CreateScale(tc.localScale) * Matrix::CreateFromYawPitchRoll(tc.localRotation) * Matrix::CreateTranslation(tc.localPosition);
	entt::entity curID = static_cast<entt::entity>(tc.parent);
	if (curID!=entt::null)
	{
		TransformComponent& curTc = context->ecs.get<TransformComponent>(curID);
		tc.transformMatrix = tc.transformMatrix * curTc.transformMatrix;
	}
	
	for (auto& child : tc.children)
	{
		UpdateTransformMatrix(context->ecs.get<TransformComponent>(child));
	}

	Quaternion q;
	Vector3 scaleNew = tc.scale;
	Vector3 positionNew = tc._position;
	tc.transformMatrix.Decompose(scaleNew, q, positionNew);
	tc._position = positionNew;
	tc.scale = scaleNew;
	tc._rotation = q.ToEuler();

	size_t seed = 0;
	boost::hash_combine(seed, tc._position);
	boost::hash_combine(seed, tc._rotation);
	boost::hash_combine(seed, tc.scale);

	tc.worldHash = seed;

}

void TransformHelper::UpdateWorldTransformMatrix(TransformComponent& tc, Matrix parentTransform)
{
	size_t seed = 0;

	//if (tc.parent==entt::null)
	//{
		Quaternion rotation;
		tc.transformMatrix = Matrix::CreateScale(tc.scale) * Matrix::CreateFromYawPitchRoll(tc._rotation) * Matrix::CreateTranslation(tc._position);
		tc.transformMatrix = tc.transformMatrix * parentTransform;
		tc.transformMatrix.Decompose(tc.localScale, rotation, tc.localPosition);

		tc.localPosition = tc._position;
		tc.localRotation = rotation.ToEuler();
		tc.localScale = tc.scale;

		//boost::hash_combine(seed, tc.localPosition);
		//boost::hash_combine(seed, tc.localRotation);
		//boost::hash_combine(seed, tc.localScale);

		//tc.localHash = seed;
	//}

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

		//size_t seed = 0;
		//boost::hash_combine(seed, tc.localPosition);
		//boost::hash_combine(seed, tc.localRotation);
		//boost::hash_combine(seed, tc.localScale);

		//tc.localHash = seed;
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

		/*size_t seed = 0;
		boost::hash_combine(seed, tc.localPosition);
		boost::hash_combine(seed, tc.localRotation);
		boost::hash_combine(seed, tc.localScale);

		tc.localHash = seed;*/
		tc.transformMatrix = (Matrix::CreateScale(scale) * Matrix::CreateFromYawPitchRoll(tc.localRotation) * Matrix::CreateTranslation(translation));
	}

}

bool TransformHelper::HasHierarchyCycles(entt::registry* ecs, entt::entity sourceGameObject, entt::entity parentGameObject)
{
	if (parentGameObject == sourceGameObject)
		return true;

	bool res = false;
	for (auto& gameObject : ecs->get<TransformComponent>(sourceGameObject).children)
	{
		res = res || HasHierarchyCycles(ecs,gameObject, parentGameObject);
	}
	return res;
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

SyVector3 TransformHelper::DegreesToRad(const SyVector3& vec)
{
	return
	{
		static_cast<float>(vec.x * M_PI / 180.0),
		static_cast<float>(vec.y * M_PI / 180.0),
		static_cast<float>(vec.z * M_PI / 180.0)
	};
}

SyVector3 TransformHelper::RadToDegrees(const SyVector3& vec)
{
	return
	{
		static_cast<float>(vec.x * 180.0 / M_PI),
		static_cast<float>(vec.y * 180.0 / M_PI),
		static_cast<float>(vec.z * 180.0 / M_PI)
	};
}

void TransformHelper::SetDefaultHash()
{
	size_t seed = 0;
	TransformComponent tc;
	boost::hash_combine(seed, tc.localPosition);
	boost::hash_combine(seed, tc.localRotation);
	boost::hash_combine(seed, tc.localScale);
	worldHashDefault = seed;
	boost::hash_combine(seed, tc.parent);
	localHashDefault = seed;

}
