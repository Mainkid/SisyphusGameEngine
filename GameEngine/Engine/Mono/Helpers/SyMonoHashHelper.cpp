#include "SyMonoHashHelper.h"

#include <boost/container_hash/hash.hpp>

#include "../../Components/TransformComponent.h"
#include "../../Features/Lighting/Components/LightComponent.h"
#include "../../Features/Mesh/Components/MeshComponent.h"
#include "../../Features/Physics/Components/ColliderComponent.h"
#include "../../Features/Physics/Components/RBodyComponent.h"

using namespace mono;

size_t SyMonoHashHelper::Hash(const TransformComponent& tf)
{
	size_t hash = 0;
	boost::hash_combine(hash, tf._position);
	boost::hash_combine(hash, tf._rotation);
	boost::hash_combine(hash, tf.scale);
	boost::hash_combine(hash, tf.parent);
	boost::hash_combine(hash, tf.localPosition);
	boost::hash_combine(hash, tf.localRotation);
	boost::hash_combine(hash, tf.localScale);
	return hash;
}

size_t SyMonoHashHelper::Hash(const MeshComponent& mesh)
{
	size_t hash = 0;
	boost::hash_combine(hash, mesh.modelUUID);
	boost::hash_combine(hash, mesh.materialUUIDs);
	return hash;
}

size_t SyMonoHashHelper::Hash(const LightComponent& light)
{
	size_t hash = 0;
	boost::hash_combine(hash, light.LightType);
	boost::hash_combine(hash, light.LightBehavior);
	boost::hash_combine(hash, light.Color.x);
	boost::hash_combine(hash, light.Color.y);
	boost::hash_combine(hash, light.Color.z);
	boost::hash_combine(hash, light.Color.w);
	boost::hash_combine(hash, light.ParamsRadiusAndAttenuation.x);
	boost::hash_combine(hash, light.ParamsRadiusAndAttenuation.y);
	boost::hash_combine(hash, light.ParamsRadiusAndAttenuation.z);
	boost::hash_combine(hash, light.ParamsRadiusAndAttenuation.w);
	boost::hash_combine(hash, light.CastShadows);
	return hash;
}

size_t SyMonoHashHelper::Hash(const SyPrimitiveColliderComponent& collider)
{
	size_t hash = 0;
	boost::hash_combine(hash, collider.ColliderType);
	boost::hash_combine(hash, collider.Extent);
	boost::hash_combine(hash, collider.Radius);
	boost::hash_combine(hash, collider.HalfHeight);
	return hash;
}

size_t SyMonoHashHelper::Hash(const SyRBodyComponent& rigid)
{
	size_t hash = 0;
	boost::hash_combine(hash, rigid.RbType);
	boost::hash_combine(hash, rigid.Mass);
	boost::hash_combine(hash, rigid.Flags);
	boost::hash_combine(hash, rigid.LinearVelocity);
	boost::hash_combine(hash, rigid.AngularVelocity);
	return hash;
}
