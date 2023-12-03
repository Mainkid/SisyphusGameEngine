#include "MonoSyncGeSystem.h"

#include "../SyMono.h"
#include "../../Components/EditorBillboardComponent.h"
#include "../Components/MonoSyncComp.h"
#include "../../Features/Mesh/Components/MeshComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Core/ServiceLocator.h"
#include "../../Contexts/EngineContext.h"
#include "../../Features/Resources/ResourceService.h"
#include "../../Core/ResourcePath.h"
#include "../../Scene/GameObjectHelper.h"
#include "../Helpers/SyMonoHashHelper.h"
#include "../../Features/Physics/Events/SyOnCreateColliderEvent.h"
#include "../../Features/Physics/Events/SyOnCreateRBodyEvent.h"
#include "../SyMonoStr.h"

SyResult MonoSyncGeSystem::Init()
{
	auto mono = ServiceLocator::instance()->Get<mono::SyMono>();
	_monoEcs = mono->GetEcs();
	_monoGame = mono->GetGame();

	_monoEcs->SetCallbackReceiver(this);

	_testTimeOnPrevFrame = std::chrono::steady_clock::now();

	return {};
}

SyResult MonoSyncGeSystem::Run()
{
	if (!_monoEcs->IsValid() || !_monoGame->IsValid())
		return {};

	if (false)
	{
		//TODO: rewrite when engine-context-time will be fixed.
		auto time = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(time - _testTimeOnPrevFrame).count() / 1000000.0f;
		_testTotalTime += deltaTime;
		_testTimeOnPrevFrame = time;

		mono::ProxyTimeData timeData;
		timeData.TotalTime = _testTotalTime;
		timeData.DeltaTime = deltaTime;

		_monoGame->EgLoopRun.Invoke(timeData);
	}

	_monoEcs->EgSyncEngineWithGame.Invoke();

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
	_ecs->emplace<GameObjectComp>(ent).Name = "C# entity";
	_ecs->emplace<MonoSyncComp>(ent);

	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "engine entity e%d created", static_cast<int>(ent));
	std::cout << "[mono] engine entity " << static_cast<uint32_t>(ent) << " created" << std::endl;

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
				_monoEcs->EgDestroyEntity.Invoke(static_cast<uint32_t>(childEnt));
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
	std::cout << "[mono] ent e" << rawEnt << " add comp " << mono::ProxyCompIdExt::ToStr(id) << std::endl;

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
		auto& light = _ecs->emplace<LightComponent>(ent);
		light.ParamsRadiusAndAttenuation = Vector4{ 1, 0, 0, 1 };
	}
	else if (id == mono::EProxyCompId::Collider)
	{
		_ecs->emplace<SyPrimitiveColliderComponent>(ent);
		CallEvent<SyOnCreateColliderEvent>("OnCreateCollider", ent);
	}
	else if (id == mono::EProxyCompId::Rigid)
	{
		_ecs->emplace<SyRBodyComponent>(ent);
		CallEvent<SyOnCreateRBodyEvent>("OnCreateRBody", ent);
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
	//std::cout << "[TEST] ent e" << rawEnt << " remove comp " << mono::ProxyCompIdExt::ToStr(id) << std::endl;

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
		_ecs->remove<LightComponent>(ent);
	}
	else if (id == mono::EProxyCompId::Collider)
	{
		_ecs->remove<SyPrimitiveColliderComponent>(ent);
	}
	else if (id == mono::EProxyCompId::Rigid)
	{
		_ecs->remove<SyRBodyComponent>(ent);
	}
	else
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "not implemented");
	}
}

void MonoSyncGeSystem::OnUpdateTransformComp(uint32_t rawEnt, const mono::ProxyTransformComp& proxy)
{
	auto ent = static_cast<entt::entity>(rawEnt);
	auto tf = _ecs->try_get<TransformComponent>(ent);
	if (tf == nullptr)
	{
		std::cout << "[mono] e" << rawEnt << "does not have transform" << std::endl;
		return;
	}

	tf->_position = proxy.Position;
	tf->_rotation = proxy.Rotation;
	tf->scale = proxy.Scale;
	tf->localPosition = proxy.LocalPosition;
	tf->localRotation = proxy.LocalRotation;
	tf->localScale = proxy.LocalScale;

	if (proxy.HasParent)
	{
		if (tf->parent != proxy.ParentEngineEnt)
			GameObjectHelper::SetParent(_ecs, ent, static_cast<entt::entity>(proxy.ParentEngineEnt));
	}
	else
	{
		if (tf->parent != entt::null)
			GameObjectHelper::SetParent(_ecs, ent, entt::null);
	}

	tf->MonoHash = mono::SyMonoHashHelper::Hash(*tf);
}

void MonoSyncGeSystem::OnUpdateMeshComp(uint32_t rawEnt, const mono::ProxyMeshComp& proxy)
{
	auto ent = static_cast<entt::entity>(rawEnt);
	auto mesh = _ecs->try_get<MeshComponent>(ent);
	if (mesh == nullptr)
	{
		std::cout << "[mono] e" << rawEnt << "does not have mesh" << std::endl;
		return;
	}

	if (proxy.ModelUuid == nullptr)
	{
		auto resService = ServiceLocator::instance()->Get<ResourceService>();
		auto uuid = resService->GetUUIDFromPath(cubeMeshPath);
		mesh->modelUUID = uuid;
	}
	else
	{
		mono::SyMonoStr strModelUuid {proxy.ModelUuid};
		mesh->modelUUID = strModelUuid.ToUuid();
	}

	mesh->materialUUIDs.clear();
	if (proxy.MaterialsUuids != nullptr)
	{
		int length = mono_array_length(proxy.MaterialsUuids);
		for (int i = 0; i < length; i++)
		{
			MonoString* rawMaterialUuid = mono_array_get(proxy.MaterialsUuids, MonoString*, i);
			if (rawMaterialUuid != nullptr)
			{
				mono::SyMonoStr strMaterialUuid {rawMaterialUuid};
				mesh->materialUUIDs.push_back(strMaterialUuid.ToUuid());
			}
		}
	}

	mesh->MonoHash = mono::SyMonoHashHelper::Hash(*mesh);
}

void MonoSyncGeSystem::OnUpdateLightComp(uint32_t rawEnt, const mono::ProxyLightComp& proxy)
{
	auto ent = static_cast<entt::entity>(rawEnt);
	auto light = _ecs->try_get<LightComponent>(ent);
	if (light == nullptr)
	{
		std::cout << "[mono] e" << rawEnt << "does not have light" << std::endl;
		return;
	}

	light->LightType = proxy.LightType;
	light->LightBehavior = proxy.LightBehavior;
	light->Color = proxy.Color;
	light->ParamsRadiusAndAttenuation.x = proxy.PointLightRadius;
	light->CastShadows = proxy.ShouldCastShadows;

	light->MonoHash = mono::SyMonoHashHelper::Hash(*light);
}

void MonoSyncGeSystem::OnUpdateColliderComp(uint32_t rawEnt, const mono::ProxyColliderComp& proxy)
{
	auto ent = static_cast<entt::entity>(rawEnt);
	auto collider = _ecs->try_get<SyPrimitiveColliderComponent>(ent);
	if (collider == nullptr)
	{
		std::cout << "[mono] e" << rawEnt << "does not have collider" << std::endl;
		return;
	}

	collider->ColliderType = proxy.Type;
	collider->Extent = proxy.Extent;
	collider->Radius = proxy.Radius;
	collider->HalfHeight = proxy.HalfHeight;

	collider->MonoHash = mono::SyMonoHashHelper::Hash(*collider);
}

void MonoSyncGeSystem::OnUpdateRigidComp(uint32_t rawEnt, const mono::ProxyRigidComp& proxy)
{
	auto ent = static_cast<entt::entity>(rawEnt);
	auto rigid = _ecs->try_get<SyRBodyComponent>(ent);
	if (rigid == nullptr)
	{
		std::cout << "[mono] e" << rawEnt << "does not have rigid" << std::endl;
		return;
	}

	rigid->RbType = proxy.Type;
	rigid->Mass = proxy.Mass;
	rigid->Flags = 0;
	rigid->Flags |= proxy.IsAutoMass ? SyERBodyFlags::USE_DENSITY : 0;
	rigid->Flags |= proxy.IsKinematic ? SyERBodyFlags::KINEMATIC : 0;
	rigid->Flags |= proxy.IsGravityOn ? 0 : SyERBodyFlags::DISABLE_GRAVITY;
	rigid->LinearVelocity = proxy.LinearVelocity;
	rigid->AngularVelocity = proxy.AngularVelocity;

	rigid->MonoHash = mono::SyMonoHashHelper::Hash(*rigid);
}