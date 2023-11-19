#include "MonoSyncEgSystem.h"

#include "../SyMono.h"
#include "../Components/MonoSyncComp.h"
#include "../../Components/TransformComponent.h"


SyResult MonoSyncEgSystem::Init()
{
	auto mono = ServiceLocator::instance()->Get<mono::SyMono>();
	_monoEcs = mono->GetEcs();

	return {};
}

SyResult MonoSyncEgSystem::Run()
{
	if (!_monoEcs->IsValid())
		return {};

	auto viewTf = _ecs->view<MonoSyncComp, TransformComponent>();
	for (auto ent : viewTf)
	{
		auto& tf = viewTf.get<TransformComponent>(ent);

		mono::ProxyTransformComp proxy;

		proxy.Position = tf._position;
		proxy.Rotation = tf._rotation;
		proxy.Scale = tf.scale;
		proxy.LocalPosition = tf.localPosition;
		proxy.LocalRotation = tf.localRotation;
		proxy.LocalScale = tf.localScale;
		proxy.HasParent = tf.parent != entt::null;
		proxy.ParentEngineEnt = tf.parent;
		_monoEcs->EgUpdateTransformComp.Invoke(static_cast<uint32_t>(ent), proxy);
	}

	return {};
}

SyResult MonoSyncEgSystem::Destroy()
{
	return {};
}
