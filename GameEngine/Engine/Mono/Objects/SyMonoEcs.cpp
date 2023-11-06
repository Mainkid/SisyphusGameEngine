#include "SyMonoEcs.h"
#include "../ISyMonoGameCallbackReceiver.h"

using namespace mono;

void SyMonoEcs::GeCreateEngineEntity()
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnCreateEngineEntity();
}

void SyMonoEcs::GeDestroyEngineEntity(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnDestroyEngineEntity(rawEnt);
}

void SyMonoEcs::GeAddTransformComp(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnAddTransformComp(rawEnt);
}

void SyMonoEcs::GeUpdateTransformComp(uint32_t rawEnt, ProxyTransformComp proxy)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnUpdateTransformComp(rawEnt, proxy);
}

void SyMonoEcs::GeRemoveTransformComp(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnRemoveTransformComp(rawEnt);
}


void SyMonoEcs::GeAddMeshComp(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnAddMeshComp(rawEnt);
}

void SyMonoEcs::GeUpdateMeshComp(uint32_t rawEnt, ProxyMeshComp proxy)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnUpdateMeshComp(rawEnt, proxy);
}

void SyMonoEcs::GeRemoveMeshComp(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnRemoveMeshComp(rawEnt);
}



void SyMonoEcs::SetCallbackReceiver(ISyMonoGameCallbackReceiver* receiver)
{
	_cbReceiver = receiver;
}



SyResult SyMonoEcs::OnAfterCreate()
{
	SyResult r{};

	_instance = this;

	SY_RESULT_CHECK(EgContinueEntityDestroyCascade.Bind(_instance));
	SY_RESULT_CHECK(EgUpdateTransformComp.Bind(_instance));
	SY_RESULT_CHECK(EgUpdateMeshComp.Bind(_instance));

	BindCallback(GE_CREATE_ENTITY, &GeCreateEngineEntity);
	BindCallback(GE_DESTROY_ENTITY, &GeDestroyEngineEntity);

	BindCallback(GE_ADD_TRANSFORM_COMP, &GeAddTransformComp);
	BindCallback(GE_UPDATE_TRANSFORM_COMP, &GeUpdateTransformComp);
	BindCallback(GE_REMOVE_TRANSFORM_COMP, &GeRemoveTransformComp);

	BindCallback(GE_ADD_MESH_COMP, &GeAddMeshComp);
	BindCallback(GE_UPDATE_MESH_COMP, &GeUpdateMeshComp);
	BindCallback(GE_REMOVE_MESH_COMP, &GeRemoveMeshComp);

	return {};
}

SyResult SyMonoEcs::OnBeforeDestroy()
{
	_instance = nullptr;

	EgContinueEntityDestroyCascade.UnBind();
	EgUpdateTransformComp.UnBind();
	EgUpdateMeshComp.UnBind();

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
