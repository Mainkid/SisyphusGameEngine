#include "SyMonoEcsSyncCamera.h"

#include "../../Features/Common/Events/CompAddedEv.h"
#include "../../Features/Common/Events/CompRemovedEv.h"
#include "../../Scene/GameObjectHelper.h"

using namespace mono;

void SyMonoEcsSyncCamera::AddComp(entt::entity ent)
{
	_ecs->emplace<CameraComponent>(ent);
	GameObjectHelper::CallEvent<CompAddedEv>(_ecs, GetCompId(), ent, true);
}

void SyMonoEcsSyncCamera::RemoveComp(entt::entity ent)
{
	_ecs->remove<CameraComponent>(ent);
	GameObjectHelper::CallEvent<CompRemovedEv>(_ecs, GetCompId(), ent, true);
}

void SyMonoEcsSyncCamera::FillProxyByComp(const CameraComponent& comp)
{
	_proxy.NearPlane = comp.nearPlane;
	_proxy.FarPlane = comp.farPlane;
	_proxy.FovDegrees = comp.fovDegrees;
}

void SyMonoEcsSyncCamera::FillCompByProxy(const ProxyCameraComp& proxy, entt::entity ent, CameraComponent& comp)
{
	comp.nearPlane = proxy.NearPlane;
	comp.farPlane = proxy.FarPlane;
	comp.fovDegrees = proxy.FovDegrees;
}

size_t SyMonoEcsSyncCamera::GetHash(const CameraComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.nearPlane);
	boost::hash_combine(hash, comp.farPlane);
	boost::hash_combine(hash, comp.fovDegrees);
	return hash;
}