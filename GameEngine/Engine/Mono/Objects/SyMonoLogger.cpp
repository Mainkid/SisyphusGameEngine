#include "SyMonoLogger.h"

#include "../Api/SyMonoStr.h"

using namespace mono;

void SyMonoLogger::GE_Log(int rawLevel, MonoString* rawTag, MonoString* rawMsg)
{
	SyElLogLevel level;
	switch (rawLevel)
	{
	case 0: level = SY_LOGLEVEL_DEBUG; break;
	case 1: level = SY_LOGLEVEL_INFO; break;
	case 2: level = SY_LOGLEVEL_WARNING; break;
	case 3: level = SY_LOGLEVEL_ERROR; break;
	default: 
		SY_LOG_GAME(SY_LOGLEVEL_WARNING, "wrong log level %d, set \"error\" level", rawLevel);
		level = SY_LOGLEVEL_ERROR;
	}

	SyMonoStr tag{ rawTag };
	SyMonoStr msg{ rawMsg };

	SY_LOG_GAME(level, "[%s] %s", tag.GetChar(), msg.GetChar());
}

SyResult SyMonoLogger::OnAfterCreate()
{
	std::string prefix = NAMESPACE + std::string(".") + CLASS_NAME + std::string("::");
	mono_add_internal_call((prefix + GE_LOG).c_str(),
		(const void*)GE_Log);

	return {};
}
SyResult SyMonoLogger::OnBeforeDestroy()
{
	return {};
}

const std::string& SyMonoLogger::GetMonoClassName()
{
	return CLASS_NAME;
}
const std::string& SyMonoLogger::GetNamespace()
{
	return NAMESPACE;
}
bool SyMonoLogger::IsUserClass()
{
	return false;
}
