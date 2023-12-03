#include "SyMonoGame.h"

#include "../SyMonoRuntime.h"

using namespace mono;

SyResult SyMonoGame::OnAfterCreate()
{
	SY_RESULT_CHECK(EgInit.Bind(this));
	SY_RESULT_CHECK(EgLoopRun.Bind(this));
	SY_RESULT_CHECK(EgLoopDestroy.Bind(this));
	SY_RESULT_CHECK(EgSaveScene.Bind(this));
	return {};
}

SyResult SyMonoGame::OnBeforeDestroy()
{
	EgInit.UnBind();
	EgLoopRun.UnBind();
	EgLoopDestroy.UnBind();
	EgSaveScene.UnBind();

	return {};
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
