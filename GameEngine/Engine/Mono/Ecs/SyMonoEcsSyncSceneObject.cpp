#include "SyMonoEcsSyncSceneObject.h"

#include "../Api/SyMonoStr.h"

using namespace mono;

void SyMonoEcsSyncSceneObject::AddComp(entt::entity ent)
{
	//impossible
}

void SyMonoEcsSyncSceneObject::RemoveComp(entt::entity ent)
{
	//impossible
}

void SyMonoEcsSyncSceneObject::FillProxyByComp(const GameObjectComp& comp)
{
	_proxy.Name = mono_string_new(_monoDomain, comp.Name.c_str());
	_proxy.IsActive = true;
}

void SyMonoEcsSyncSceneObject::FillCompByProxy(
	const ProxySceneObjectComp& proxy, 
	entt::entity ent,
	GameObjectComp& comp)
{
	SyMonoStr name{ proxy.Name };
	comp.Name = name.ToStr();
}

size_t SyMonoEcsSyncSceneObject::GetHash(const GameObjectComp& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.Name);
	return hash;
}
