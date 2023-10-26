#include "SyMonoGameConfig.h"


using namespace mono;

void SyMonoGameConfig::OnAfterCreate() {}

void SyMonoGameConfig::OnBeforeDestroy() {}

const std::string& SyMonoGameConfig::GetMonoClassName()
{
	return CLASS_NAME;
}

const std::string& SyMonoGameConfig::GetNamespace()
{
	return NAMESPACE;
}

bool SyMonoGameConfig::IsUserClass()
{
	return true;
}
