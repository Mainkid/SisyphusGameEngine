#include "MonoSyncSystem.h"

#include "../../Features/Mesh/Components/MeshComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Core/ServiceLocator.h"
#include "../../Contexts/EngineContext.h"
#include "../../Features/Resources/ResourceService.h"
#include "../Components/MonoSyncComp.h"
#include "../../Core/ResourcePath.h"
#include "../../Scene/GameObjectHelper.h"

SyResult MonoSyncSystem::Init()
{
	auto mono = ServiceLocator::instance()->Get<mono::SyMono>();
	_monoEcs = mono->GetEcs();
	_monoGame = mono->GetGame();

	if (_monoEcs->IsValid())
		_monoEcs->SetCallbackReceiver(this);
	else
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "failed to attach callbacks due to mono ecs invalid");

	_testTimeOnPrevFrame = std::chrono::steady_clock::now();

	return {};
}

SyResult MonoSyncSystem::Run()
{
	if (!_monoEcs->IsValid() || !_monoGame->IsValid())
		return {};

	if (!_isGameInited)
	{
		_monoGame->EgLoopInit.Invoke();
		_isGameInited = true;
	}

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

	//TODO: rewrite when engine-context-time will be fixed.
	auto time = std::chrono::steady_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(time - _testTimeOnPrevFrame).count() / 1000000.0f;
	_testTotalTime += deltaTime;
	_testTimeOnPrevFrame = time;

	mono::ProxyTimeData timeData;
	timeData.TotalTime = _testTotalTime;
	timeData.DeltaTime = deltaTime;

	_monoGame->EgLoopRun.Invoke(timeData);

	return {};
}

SyResult MonoSyncSystem::Destroy()
{
	if (_monoGame->IsValid())
		_monoGame->EgLoopDestroy.Invoke();

	return {};
}


uint32_t MonoSyncSystem::OnCreateEngineEntity()
{
	auto ent = _ecs->create();
	_ecs->emplace<GameObjectComp>(ent).Name = "FromC#";
	_ecs->emplace<MonoSyncComp>(ent);

	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "engine entity e%d created", static_cast<int>(ent));

	return static_cast<uint32_t>(ent);
}

void MonoSyncSystem::OnDestroyEngineEntity(uint32_t rawEnt)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "engine entity e%d destroy", static_cast<int>(rawEnt));

	OnDestroyEngineEntityImpl(static_cast<entt::entity>(rawEnt), false);
}

void MonoSyncSystem::OnDestroyEngineEntityImpl(entt::entity ent, bool isRecursionStep)
{
	auto tf = _ecs->try_get<TransformComponent>(ent);
	if (tf != nullptr)
	{
		if (tf->children.size() > 0)
		{
			for (auto childEnt : tf->children)
			{
				SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "continue entity destroy cascade, e%d destroy", static_cast<int>(childEnt));
				OnDestroyEngineEntityImpl(childEnt, true);
				_monoEcs->EgContinueEntityDestroyCascade.Invoke(static_cast<uint32_t>(childEnt));
			}
		}
		if (!isRecursionStep)
			GameObjectHelper::SetParent(_ecs, ent, entt::null);
	}
	_ecs->destroy(ent);
}


void MonoSyncSystem::OnAddTransformComp(uint32_t rawEnt)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "add transform comp to e%d", static_cast<int>(rawEnt));

	_ecs->emplace<TransformComponent>(static_cast<entt::entity>(rawEnt));
}

void MonoSyncSystem::OnRemoveTransformComp(uint32_t rawEnt)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "remove transform comp from e%d", static_cast<int>(rawEnt));

	auto ent = static_cast<entt::entity>(rawEnt);
	GameObjectHelper::SetParent(_ecs, ent, entt::null);
	_ecs->remove<TransformComponent>(ent);
}

void MonoSyncSystem::OnUpdateTransformComp(uint32_t rawEnt, const mono::ProxyTransformComp& proxy)
{
	auto ent = static_cast<entt::entity>(rawEnt);

	auto& tf = _ecs->get<TransformComponent>(ent);
	tf.localPosition = proxy.LocalPosition;
	tf.localRotation = proxy.LocalRotation;
	tf.localScale = proxy.LocalScale;

	if (proxy.HasParent)
	{
		if (tf.parent != proxy.ParentEngineEnt)
			GameObjectHelper::SetParent(_ecs, ent, static_cast<entt::entity>(proxy.ParentEngineEnt));
	}
	else
	{
		if (tf.parent != entt::null)
			GameObjectHelper::SetParent(_ecs, ent, entt::null);
	}
}


void MonoSyncSystem::OnAddMeshComp(uint32_t rawEnt)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "add mesh comp to e%d", static_cast<int>(rawEnt));

	auto resourceService = ServiceLocator::instance()->Get<ResourceService>();
	auto uuid = resourceService->GetUUIDFromPath(cubeMeshPath);

	_ecs->emplace<MeshComponent>(static_cast<entt::entity>(rawEnt))
		.modelUUID = uuid;
}

void MonoSyncSystem::OnRemoveMeshComp(uint32_t rawEnt)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "remove mesh comp from e%d", static_cast<int>(rawEnt));

	_ecs->remove<MeshComponent>(static_cast<entt::entity>(rawEnt));
}

void MonoSyncSystem::OnUpdateMeshComp(uint32_t rawEnt, const mono::ProxyMeshComp& proxy)
{
	auto& mesh = _ecs->get<MeshComponent>(static_cast<entt::entity>(rawEnt));
	//mesh.texturePath = proxy.TexturePath;
	//mesh.modelPath = proxy.ModelPath;
}