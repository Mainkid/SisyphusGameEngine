#include "SyMonoObj.h"

#include <iostream>

#include "SyMonoRuntime.h"


using namespace mono;

SyMonoObj::~SyMonoObj()
{
	Destroy();
}

void SyMonoObj::Create(const SyMonoRuntime& runtime)
{
	MonoImage* image = IsUserClass() ? runtime.GetUserAssemblyImage() : runtime.GetLayerAssemblyImage();

	_class = mono_class_from_name(
		image,
		GetNamespace().c_str(),
		GetMonoClassName().c_str());
	if (_class == nullptr)
	{
		std::cout << "[mono] failed to find class " << GetMonoClassName() << std::endl;
		return;
	}

	MonoObject* instance = mono_object_new(
		runtime.GetDomain(),
		_class);
	if (instance == nullptr)
	{
		std::cout << "[mono] failed to instantiate object " << GetMonoClassName() << std::endl;
	}

	mono_runtime_object_init(instance);
	_gcHandle = mono_gchandle_new(instance, false);
	_isValid = true;

	OnAfterCreate();
}

void SyMonoObj::Destroy()
{
	if (!IsValid())
		return;

	OnBeforeDestroy();

	_class = nullptr;
	_isValid = false;
	mono_gchandle_free(_gcHandle);
}

MonoClass* SyMonoObj::GetClass() const
{
	return _class;
}

MonoObject* SyMonoObj::GetInstance() const
{
	return mono_gchandle_get_target(_gcHandle);
}


MonoMethod* SyMonoObj::FindMethod(const std::string& methodName, int paramsCount)
{
	MonoMethod* method = mono_class_get_method_from_name(
		_class,
		methodName.c_str(),
		paramsCount);
	if (method == nullptr)
		std::cout << "[mono] failed to find method " << methodName << std::endl;
	return method;
}

void* SyMonoObj::BindMethod(const std::string& methodName, int paramsCount)
{
	MonoMethod* method = FindMethod(methodName, paramsCount);
	if (method == nullptr)
		return nullptr;
	return mono_method_get_unmanaged_thunk(method);
}

bool SyMonoObj::IsValid() const
{
	return _isValid;
}
