#include "SyMonoObj.h"

#include <iostream>

#include "SyMonoRuntime.h"


using namespace mono;


SyResult SyMonoObj::Create(const SyMonoRuntime& runtime)
{
	MonoImage* image = IsUserClass() ? runtime.GetUserAssemblyImage() : runtime.GetLayerAssemblyImage();

	_class = mono_class_from_name(
		image,
		GetNamespace().c_str(),
		GetMonoClassName().c_str());
	if (_class == nullptr)
		return SyResult{ SY_RESCODE_ERROR, SyComplexString("failed to find class %s", GetMonoClassName().c_str()) };

	MonoObject* instance = mono_object_new(
		runtime.GetDomain(),
		_class);
	if (instance == nullptr)
		return SyResult{ SY_RESCODE_ERROR, SyComplexString("obj is not instantiated %s", GetMonoClassName().c_str()) };

	mono_runtime_object_init(instance);
	_gcHandle = mono_gchandle_new(instance, false);
	_isValid = true;

	return OnAfterCreate();
}

SyResult SyMonoObj::Destroy()
{
	if (!IsValid())
		return {};

	SyResult r = OnBeforeDestroy();

	_class = nullptr;
	_isValid = false;
	mono_gchandle_free(_gcHandle);

	return r;
}

MonoClass* SyMonoObj::GetClass() const
{
	return _class;
}

MonoObject* SyMonoObj::GetInstance() const
{
	return mono_gchandle_get_target(_gcHandle);
}

void SyMonoObj::BindCallback(const std::string& name, const void* cb)
{
	std::string fullName = GetNamespace() + std::string(".") +
		GetMonoClassName() + std::string("::") +
		name;
	mono_add_internal_call(fullName.c_str(), cb);
}

bool SyMonoObj::IsValid() const
{
	return _isValid;
}
