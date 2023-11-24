#include "MonoSyncGeSystem.h"

#include "../SyMono.h"
#include "../Components/MonoSyncComp.h"
#include "../../Components/MeshComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Core/ServiceLocator.h"
#include "../../Systems/EngineContext.h"
#include "../../Systems/ResourceService.h"
#include "../../Core/ResourcePath.h"
#include "../../Scene/GameObjectHelper.h"

SyResult MonoSyncGeSystem::Init()
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

SyResult MonoSyncGeSystem::Run()
{
	if (!_monoEcs->IsValid() || !_monoGame->IsValid())
		return {};

	if (!_isGameInited)
	{
		_monoGame->EgLoopInit.Invoke();
		_isGameInited = true;
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

SyResult MonoSyncGeSystem::Destroy()
{
	if (_monoGame->IsValid())
		_monoGame->EgLoopDestroy.Invoke();

	return {};
}


uint32_t MonoSyncGeSystem::OnCreateEntity()
{
	auto ent = _ecs->create();
	_ecs->emplace<GameObjectComp>(ent).Name = "FromC#";
	_ecs->emplace<MonoSyncComp>(ent);

	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "engine entity e%d created", static_cast<int>(ent));

	return static_cast<uint32_t>(ent);
}

void MonoSyncGeSystem::OnDestroyEntity(uint32_t rawEnt)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "engine entity e%d destroy", static_cast<int>(rawEnt));

	OnDestroyEngineEntityImpl(static_cast<entt::entity>(rawEnt), false);
}

void MonoSyncGeSystem::OnDestroyEngineEntityImpl(entt::entity ent, bool isRecursionStep)
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

void MonoSyncGeSystem::OnAddComp(uint32_t rawEnt, mono::EProxyCompId id)
{
	SY_LOG_MONO(
		SY_LOGLEVEL_DEBUG, 
		"add %s to e%d", 
		mono::ProxyCompIdExt::ToStr(id).c_str(),
		static_cast<int>(rawEnt)
	);

	auto ent = static_cast<entt::entity>(rawEnt);
	if (id == mono::EProxyCompId::Transform)
	{
		_ecs->emplace<TransformComponent>(ent);
	}
	else if (id == mono::EProxyCompId::Mesh)
	{
		auto resService = ServiceLocator::instance()->Get<ResourceService>();
		auto uuid = resService->GetUUIDFromPath(cubeMeshPath);
		_ecs->emplace<MeshComponent>(ent).modelUUID = uuid;
	}
	else if (id == mono::EProxyCompId::Light)
	{
		_ecs->emplace<LightComponent>(ent);
	}
	else if (id == mono::EProxyCompId::Rigid)
	{
		
	}
	else
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "not implemented");
	}
}

void MonoSyncGeSystem::OnRemoveComp(uint32_t rawEnt, mono::EProxyCompId id)
{
	SY_LOG_MONO(
		SY_LOGLEVEL_DEBUG, 
		"remove %s from e%d",
		mono::ProxyCompIdExt::ToStr(id).c_str(),
		static_cast<int>(rawEnt)
	);

	auto ent = static_cast<entt::entity>(rawEnt);
	if (id == mono::EProxyCompId::Transform)
	{
		GameObjectHelper::SetParent(_ecs, ent, entt::null);
		_ecs->remove<TransformComponent>(ent);
	}
	else if (id == mono::EProxyCompId::Mesh)
	{
		_ecs->remove<MeshComponent>(ent);
	}
	else if (id == mono::EProxyCompId::Light)
	{
		_ecs->emplace<LightComponent>(ent);
	}
	else if (id == mono::EProxyCompId::Rigid)
	{
	}
	else
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "not implemented");
	}
}

void MonoSyncGeSystem::OnUpdateTransformComp(uint32_t rawEnt, const mono::ProxyTransformComp& proxy)
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

	tf.MonoHash = std::hash<TransformComponent>{}(tf);
}

void MonoSyncGeSystem::OnUpdateMeshComp(uint32_t rawEnt, const mono::ProxyMeshComp& proxy)
{
	auto& mesh = _ecs->get<MeshComponent>(static_cast<entt::entity>(rawEnt));

	if (proxy.ModelUuid == nullptr)
	{
		auto resService = ServiceLocator::instance()->Get<ResourceService>();
		auto uuid = resService->GetUUIDFromPath(cubeMeshPath);
		mesh.modelUUID = uuid;
	}
	else
	{
		char* buffModelUuid = mono_string_to_utf8(proxy.ModelUuid);
		auto strModelUuid = std::string(buffModelUuid);
		mesh.modelUUID = boost::lexical_cast<boost::uuids::uuid>(strModelUuid);
	}

	mesh.materialUUIDs.clear();
	if (proxy.MaterialsUuids != nullptr)
	{
		int length = mono_array_length(proxy.MaterialsUuids);
		for (int i = 0; i < length; i++)
		{
			MonoString* rawMaterialUuid = mono_array_get(proxy.MaterialsUuids, MonoString*, i);
			if (rawMaterialUuid != nullptr)
			{
				char* buffMaterialUuid = mono_string_to_utf8(rawMaterialUuid);
				auto strMaterialUuid = std::string(buffMaterialUuid);
				mesh.materialUUIDs.push_back(boost::lexical_cast<boost::uuids::uuid>(strMaterialUuid));
			}
		}
	}

	mesh.MonoHash = std::hash<MeshComponent>{}(mesh);
}

void MonoSyncGeSystem::OnUpdateLightComp(uint32_t rawEnt, const mono::ProxyLightComp& proxy) {}