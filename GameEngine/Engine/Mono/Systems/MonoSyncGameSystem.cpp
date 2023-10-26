#include "MonoSyncGameSystem.h"

#include "../../Components/MeshComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Core/ServiceLocator.h"
#include "../../Systems/EngineContext.h"
#include "../Components/MonoSyncComp.h"

SyResult MonoSyncGameSystem::Init()
{
	_engineContext = ServiceLocator::instance()->Get<EngineContext>();

	auto mono = ServiceLocator::instance()->Get<mono::SyMono>();
	_monoGame = mono->GetGame();
	if (!_monoGame->IsValid())
		return {};

	_monoGame->SetCallbackReceiver(this);

	_testTimeOnPrevFrame = std::chrono::steady_clock::now();

	return {};
}

SyResult MonoSyncGameSystem::Run()
{
	if (!_monoGame->IsValid())
		return {};

	if (!_isGameInited)
	{
		_monoGame->EG_LoopInit.Invoke();
		_isGameInited = true;
	}

	auto viewTf = _ecs->view<MonoSyncComp, TransformComponent>();
	for (auto ent : viewTf)
	{
		auto& tf = viewTf.get<TransformComponent>(ent);

		mono::ProxyTransformComp proxy;
		proxy.Position = tf.position;
		proxy.Rotation = tf.rotation;
		proxy.Scale = tf.scale;
		_monoGame->EG_UpdateTransformComp.Invoke(static_cast<uint32_t>(ent), proxy);
	}

	//TODO: rewrite when engine-context-time will be fixed.
	auto time = std::chrono::steady_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(time - _testTimeOnPrevFrame).count() / 1000000.0f;
	_testTotalTime += deltaTime;
	_testTimeOnPrevFrame = time;

	mono::ProxyTimeData timeData;
	timeData.TotalTime = _testTotalTime;
	timeData.DeltaTime = deltaTime;

	_monoGame->EG_LoopRun.Invoke(timeData);

	return {};
}

SyResult MonoSyncGameSystem::Destroy()
{
	if (_monoGame->IsValid())
		_monoGame->EG_LoopDestroy.Invoke();

	return {};
}


uint32_t MonoSyncGameSystem::CreateEngineEntity()
{
	auto ent = _ecs->create();
	_ecs->emplace<MonoSyncComp>(ent);

	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "engine entity e%d created", static_cast<int>(ent));

	return static_cast<uint32_t>(ent);
}

void MonoSyncGameSystem::DestroyEngineEntity(uint32_t rawEnt)
{
	auto ent = static_cast<entt::entity>(rawEnt);
	_ecs->destroy(ent);

	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "engine entity e%d destroyed", static_cast<int>(rawEnt));
}


void MonoSyncGameSystem::AddTransformComp(uint32_t rawEnt)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "add transform comp to e%d", static_cast<int>(rawEnt));

	_ecs->emplace<TransformComponent>(static_cast<entt::entity>(rawEnt));
}

void MonoSyncGameSystem::RemoveTransformComp(uint32_t rawEnt)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "remove transform comp from e%d", static_cast<int>(rawEnt));

	_ecs->remove<TransformComponent>(static_cast<entt::entity>(rawEnt));
}

void MonoSyncGameSystem::UpdateTransformComp(uint32_t rawEnt, const mono::ProxyTransformComp& proxy)
{
	auto& tf = _ecs->get<TransformComponent>(static_cast<entt::entity>(rawEnt));
	tf.position = proxy.Position;
	tf.rotation = proxy.Rotation;
	tf.scale = proxy.Scale;
	tf.localPosition = proxy.Position;
	tf.localRotation = proxy.Rotation;
	tf.localScale = proxy.Scale;
}


void MonoSyncGameSystem::AddMeshComp(uint32_t rawEnt)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "add mesh comp to e%d", static_cast<int>(rawEnt));

	_ecs->emplace<MeshComponent>(static_cast<entt::entity>(rawEnt));
}

void MonoSyncGameSystem::RemoveMeshComp(uint32_t rawEnt)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "remove mesh comp from e%d", static_cast<int>(rawEnt));

	_ecs->remove<MeshComponent>(static_cast<entt::entity>(rawEnt));
}

void MonoSyncGameSystem::UpdateMeshComp(uint32_t rawEnt, const mono::ProxyMeshComp& proxy)
{
	auto& mesh = _ecs->get<MeshComponent>(static_cast<entt::entity>(rawEnt));
	mesh.texturePath = proxy.TexturePath;
	mesh.modelPath = proxy.ModelPath;
}