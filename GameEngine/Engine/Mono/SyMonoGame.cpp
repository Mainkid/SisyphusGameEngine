#include "SyMonoGame.h"

#include <iostream>

#include "ISyMonoGameCallbackReceiver.h"
#include "SyMonoRuntime.h"

using namespace mono;


void SyMonoGame::GE_CreateEngineEntity()
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->CreateEngineEntity();
}

void SyMonoGame::GE_DestroyEngineEntity(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->DestroyEngineEntity(rawEnt);
}

void SyMonoGame::GE_AddTransformComp(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->AddTransformComp(rawEnt);
}

void SyMonoGame::GE_UpdateTransformComp(uint32_t rawEnt, ProxyTransformComp proxy)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->UpdateTransformComp(rawEnt, proxy);
}

void SyMonoGame::GE_RemoveTransformComp(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->RemoveTransformComp(rawEnt);
}


void SyMonoGame::GE_AddMeshComp(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->AddMeshComp(rawEnt);
}

void SyMonoGame::GE_UpdateMeshComp(uint32_t rawEnt, ProxyMeshComp proxy)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->UpdateMeshComp(rawEnt, proxy);
}

void SyMonoGame::GE_RemoveMeshComp(uint32_t rawEnt)
{
	if (_instance != nullptr && _instance->_cbReceiver != nullptr)
		_instance->_cbReceiver->RemoveMeshComp(rawEnt);
}


void SyMonoGame::SetCallbackReceiver(ISyMonoGameCallbackReceiver* receiver)
{
	_cbReceiver = receiver;
}

void SyMonoGame::OnAfterCreate()
{
	_instance = this;

	EG_SetConfig.Bind(_instance);
	EG_LoopInit.Bind(_instance);
	EG_LoopRun.Bind(_instance);
	EG_LoopDestroy.Bind(_instance);
	EG_UpdateTransformComp.Bind(_instance);

	std::string prefix = NAMESPACE + std::string(".") + CLASS_NAME + std::string("::");
	mono_add_internal_call((prefix + GE_CREATE_ENTITY).c_str(),
		(const void*)GE_CreateEngineEntity);

	mono_add_internal_call((prefix + GE_ADD_TRANSFORM_COMP).c_str(),
		(const void*)GE_AddTransformComp);
	mono_add_internal_call((prefix + GE_UPDATE_TRANSFORM_COMP).c_str(),
		(const void*)GE_UpdateTransformComp);

	mono_add_internal_call((prefix + GE_ADD_MESH_COMP).c_str(),
		(const void*)GE_AddMeshComp);
	mono_add_internal_call((prefix + GE_UPDATE_MESH_COMP).c_str(),
		(const void*)GE_UpdateMeshComp);
}

void SyMonoGame::OnBeforeDestroy()
{
	_instance = nullptr;

	EG_SetConfig.UnBind();
	EG_LoopInit.UnBind();
	EG_LoopRun.UnBind();
	EG_LoopDestroy.UnBind();
	EG_UpdateTransformComp.UnBind();
}

const std::string& SyMonoGame::GetMonoClassName()
{
	return CLASS_NAME;
}

const std::string& SyMonoGame::GetNamespace()
{
	return NAMESPACE;
}

bool SyMonoGame::IsUserClass()
{
	return false;
}
