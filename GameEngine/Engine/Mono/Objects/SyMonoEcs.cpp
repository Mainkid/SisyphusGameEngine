#include "SyMonoEcs.h"
#include "ISyMonoEcsCallbacks.h"

using namespace mono;

void SyMonoEcs::GeCreateEntity()
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnCreateEntity();
}

void SyMonoEcs::GeDestroyEntity(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnDestroyEntity(rawEnt);
}

void SyMonoEcs::GeAddComp(uint32_t rawEnt, EProxyCompId id)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnAddComp(rawEnt, id);
}
void SyMonoEcs::GeRemoveComp(uint32_t rawEnt, EProxyCompId id)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnRemoveComp(rawEnt, id);
}


void SyMonoEcs::GeUpdateTransformComp(uint32_t rawEnt, ProxyTransformComp proxy)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnUpdateTransformComp(rawEnt, proxy);
}

void SyMonoEcs::GeUpdateMeshComp(uint32_t rawEnt, ProxyMeshComp proxy)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnUpdateMeshComp(rawEnt, proxy);
}

void SyMonoEcs::GeUpdateLightComp(uint32_t rawEnt, ProxyLightComp proxy)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnUpdateLightComp(rawEnt, proxy);
}

void SyMonoEcs::GeUpdateColliderComp(uint32_t rawEnt, ProxyColliderComp proxy)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnUpdateColliderComp(rawEnt, proxy);
}

void SyMonoEcs::GeUpdateRigidComp(uint32_t rawEnt, ProxyRigidComp proxy)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->OnUpdateRigidComp(rawEnt, proxy);
}


void SyMonoEcs::SetCallbackReceiver(ISyMonoEcsCallbacks* receiver)
{
	_cbReceiver = receiver;
}



SyResult SyMonoEcs::OnAfterCreate()
{
	SyResult r{};

	_instance = this;

	SY_RESULT_CHECK(EgContinueEntityDestroyCascade.Bind(this));
	SY_RESULT_CHECK(EgUpdateTransformComp.Bind(this));
	SY_RESULT_CHECK(EgUpdateMeshComp.Bind(this));
	SY_RESULT_CHECK(EgUpdateLightComp.Bind(this));

	BindCallback(GE_CREATE_ENTITY, &GeCreateEntity);
	BindCallback(GE_DESTROY_ENTITY, &GeDestroyEntity);

	BindCallback(GE_ADD_COMP, &GeAddComp);
	BindCallback(GE_REMOVE_COMP, &GeRemoveComp);

	BindCallback(GE_UPDATE_TRANSFORM_COMP, &GeUpdateTransformComp);
	BindCallback(GE_UPDATE_MESH_COMP, &GeUpdateMeshComp);
	BindCallback(GE_UPDATE_LIGHT_COMP, &GeUpdateLightComp);
	BindCallback(GE_UPDATE_COLLIDER_COMP, &GeUpdateColliderComp);
	BindCallback(GE_UPDATE_RIGID_COMP, &GeUpdateRigidComp);

	return {};
}

SyResult SyMonoEcs::OnBeforeDestroy()
{
	_instance = nullptr;

	EgContinueEntityDestroyCascade.UnBind();
	EgUpdateTransformComp.UnBind();
	EgUpdateMeshComp.UnBind();
	EgUpdateLightComp.UnBind();

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
