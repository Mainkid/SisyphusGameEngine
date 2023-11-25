#include "SyMonoHashHelper.h"

#include <boost/container_hash/hash.hpp>

#include "../../Components/TransformComponent.h"
#include "../../Features/Mesh/Components/MeshComponent.h"
#include "../../Features/Physics/Components/RBodyComponent.h"

using namespace mono;

size_t SyMonoHashHelper::Hash(const TransformComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp._position);
	boost::hash_combine(hash, comp._rotation);
	boost::hash_combine(hash, comp.scale);
	boost::hash_combine(hash, comp.parent);
	boost::hash_combine(hash, comp.localPosition);
	boost::hash_combine(hash, comp.localRotation);
	boost::hash_combine(hash, comp.localScale);
	return hash;
}

size_t SyMonoHashHelper::Hash(const MeshComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.modelUUID);
	boost::hash_combine(hash, comp.materialUUIDs);
	return hash;
}

size_t SyMonoHashHelper::Hash(const SyRBodyComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.RbType);
	boost::hash_combine(hash, comp.Mass);
	boost::hash_combine(hash, comp.Flags);
	boost::hash_combine(hash, comp.LinearVelocity);
	boost::hash_combine(hash, comp.AngularVelocity);
	return hash;
}
