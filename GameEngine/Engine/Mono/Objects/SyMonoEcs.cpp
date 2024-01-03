#include "SyMonoEcs.h"
#include "../../Components/GameObjectComp.h"
#include "../../Components/TransformComponent.h"
#include "../../Scene/GameObjectHelper.h"
#include "../Ecs/SyMonoEcsSyncCollider.h"
#include "../Ecs/SyMonoEcsSyncLight.h"
#include "../Ecs/SyMonoEcsSyncMesh.h"
#include "../Ecs/SyMonoEcsSyncParticles.h"
#include "../Ecs/SyMonoEcsSyncRigid.h"
#include "../Ecs/SyMonoEcsSyncSceneObject.h"
#include "../Ecs/SyMonoEcsSyncSkybox.h"
#include "../Ecs/SyMonoEcsSyncSound.h"
#include "../Ecs/SyMonoEcsSyncTransform.h"

using namespace mono;

uint32_t SyMonoEcs::GeCreateEntity()
{
	if (_instance == nullptr)
		return 0;
	auto ecs = _instance->_ecs;
	if (ecs == nullptr)
		return 0;

	auto ent = ecs->create();
	ecs->emplace<GameObjectComp>(ent).Name = "C# entity";
	ecs->emplace<TransformComponent>(ent);

	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "engine entity e%d created", static_cast<int>(ent));
	std::cout << "[mono] engine entity " << static_cast<uint32_t>(ent) << " created" << std::endl;

	return static_cast<uint32_t>(ent);
}

void SyMonoEcs::GeDestroyEntity(uint32_t rawEnt)
{
	if (_instance == nullptr || _instance->_ecs == nullptr)
		return;

	_instance->DestroyEntity(static_cast<entt::entity>(rawEnt), false);
}

void SyMonoEcs::GeAddComp(uint32_t rawEnt, ECompId id)
{
	if (_instance == nullptr || _instance->_ecs == nullptr)
		return;

	auto ent = static_cast<entt::entity>(rawEnt);
	auto sync = _instance->GetSync(id);
	sync->AddComp(ent);
	sync->SendDirectly(ent);

}
void SyMonoEcs::GeRemoveComp(uint32_t rawEnt, ECompId id)
{
	if (_instance == nullptr || _instance->_ecs == nullptr)
		return;

	_instance->GetSync(id)->RemoveComp(static_cast<entt::entity>(rawEnt));
}


void SyMonoEcs::BindEcs(entt::registry* ecs)
{
	_ecs = ecs;
}

ISyMonoEcsSync* SyMonoEcs::GetSync(ECompId id)
{
	return _compIdToSync[id];
}

void SyMonoEcs::DestroyEntity(entt::entity ent, bool isRecursionStep)
{
	SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "engine entity e%d destroy", static_cast<int>(ent));

	auto tf = _ecs->try_get<TransformComponent>(ent);
	if (tf != nullptr)
	{
		if (tf->children.size() > 0)
		{
			for (auto childEnt : tf->children)
			{
				SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "continue entity destroy cascade, e%d destroy", static_cast<int>(childEnt));
				DestroyEntity(childEnt, true);
				_egDestroyEntity.Invoke(static_cast<uint32_t>(childEnt));
			}
		}
		if (!isRecursionStep)
			GameObjectHelper::SetParent(_ecs, ent, entt::null);
	}
	_ecs->destroy(ent);
}

SyResult SyMonoEcs::OnAfterCreate()
{
	SyResult r{};

	_instance = this;

	SY_RESULT_CHECK(EgSyncEngineWithGame.Bind(this));
	SY_RESULT_CHECK(_egDestroyEntity.Bind(this));

	BindCallback(GE_CREATE_ENTITY, &GeCreateEntity);
	BindCallback(GE_DESTROY_ENTITY, &GeDestroyEntity);

	BindCallback(GE_ADD_COMP, &GeAddComp);
	BindCallback(GE_REMOVE_COMP, &GeRemoveComp);

	AddSync<SyMonoEcsSyncSceneObject>();
	AddSync<SyMonoEcsSyncTransform>();
	AddSync<SyMonoEcsSyncMesh>();
	AddSync<SyMonoEcsSyncLight>();
	AddSync<SyMonoEcsSyncCollider>();
	AddSync<SyMonoEcsSyncRigid>();
	AddSync<SyMonoEcsSyncSkybox>();
	AddSync<SyMonoEcsSyncParticles>();
	AddSync<SyMonoEcsSyncSound>();

	return {};
}

SyResult SyMonoEcs::OnBeforeDestroy()
{
	_instance = nullptr;

	EgSyncEngineWithGame.UnBind();
	_egDestroyEntity.UnBind();

	for (auto pair : _syncTypeToSync)
		delete pair.second;
	_syncTypeToSync.clear();
	_compIdToSync.clear();

	return {};
}

const std::string& SyMonoEcs::GetMonoClassName()
{
	return CLASS_NAME;
}

const std::string& SyMonoEcs::GetNamespace()
{
	return NAMESPACE;
}

bool SyMonoEcs::IsUserClass()
{
	return false;
}
