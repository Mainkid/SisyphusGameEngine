#include "MonoSyncSystem.h"

#include "../../Components/MeshComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Core/ServiceLocator.h"
#include "../../Systems/EngineContext.h"
#include "../Components/MonoSyncComp.h"

SyResult MonoSyncSystem::Init()
{
	_engineContext = ServiceLocator::instance()->Get<EngineContext>();

	auto mono = ServiceLocator::instance()->Get<mono::SyMono>();
	_monoGame = mono->GetGame();
	if (!_monoGame->IsValid())
		return {};

	_monoGame->SetCallbackReceiver(this);
	_monoGame->EG_LoopInit.Invoke();

	return {};
}

SyResult MonoSyncSystem::Run()
{
	if (!_monoGame->IsValid())
		return {};

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

	mono::ProxyTimeData timeData;
	timeData.TotalTime = _engineContext->totalTime;
	timeData.DeltaTime = _engineContext->deltaTime;

	_monoGame->EG_LoopRun.Invoke(timeData);

	return {};
}

SyResult MonoSyncSystem::Destroy()
{
	if (_monoGame->IsValid())
		_monoGame->EG_LoopDestroy.Invoke();

	return {};
}


uint32_t MonoSyncSystem::CreateEngineEntity()
{
	auto ent = _ecs->create();
	_ecs->emplace<MonoSyncComp>(ent);

	std::cout << "[mono] engine entity e" << static_cast<uint32_t>(ent) << " created" << std::endl;

	return static_cast<uint32_t>(ent);
}

void MonoSyncSystem::DestroyEngineEntity(uint32_t rawEnt)
{
	auto ent = static_cast<entt::entity>(rawEnt);
	_ecs->destroy(ent);
}


void MonoSyncSystem::AddTransformComp(uint32_t rawEnt)
{
	std::cout << "[mono] add transform comp to e" << rawEnt << std::endl;
	_ecs->emplace<TransformComponent>(static_cast<entt::entity>(rawEnt));
}

void MonoSyncSystem::RemoveTransformComp(uint32_t rawEnt)
{
	std::cout << "[mono] remove transform comp from e" << rawEnt << std::endl;
	_ecs->remove<TransformComponent>(static_cast<entt::entity>(rawEnt));
}

void MonoSyncSystem::UpdateTransformComp(uint32_t rawEnt, const mono::ProxyTransformComp& proxy)
{
	auto& tf = _ecs->get<TransformComponent>(static_cast<entt::entity>(rawEnt));
	tf.position = proxy.Position;
	tf.rotation = proxy.Rotation;
	tf.scale = proxy.Scale;
}


void MonoSyncSystem::AddMeshComp(uint32_t rawEnt)
{
	std::cout << "[mono] add mesh comp to e" << rawEnt << std::endl;
	_ecs->emplace<MeshComponent>(static_cast<entt::entity>(rawEnt));
}

void MonoSyncSystem::RemoveMeshComp(uint32_t rawEnt)
{
	std::cout << "[mono] remove mesh comp to e" << rawEnt << std::endl;
	_ecs->remove<MeshComponent>(static_cast<entt::entity>(rawEnt));
}

void MonoSyncSystem::UpdateMeshComp(uint32_t rawEnt, const mono::ProxyMeshComp& proxy)
{
	auto& mesh = _ecs->get<MeshComponent>(static_cast<entt::entity>(rawEnt));
	mesh.texturePath = proxy.TexturePath;
	mesh.modelPath = proxy.ModelPath;
}