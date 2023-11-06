#include "SyMonoGameConfig.h"

using namespace mono;

SyResult SyMonoGameConfig::OnAfterCreate()
{
	return {};
}

SyResult SyMonoGameConfig::OnBeforeDestroy()
{
	return {};
}

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
