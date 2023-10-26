#include "SyMono.h"


using namespace mono;

SyMono::~SyMono()
{
	Destroy();
}

void SyMono::Init()
{
	SY_LOG_MONO(SY_LOGLEVEL_INFO, "init start..");

	SyResult r = InitImpl();
	if (r.code == SY_RESCODE_OK)
	{
		SY_LOG_MONO(SY_LOGLEVEL_INFO, "init done");
	}
	else
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "init failed; %s", r.message.ToString());
		ForceDestroy();
	}
}

void SyMono::Destroy()
{
	SY_LOG_MONO(SY_LOGLEVEL_INFO, "destroy start..");

	SyResult r = DestroyImpl();
	if (r.code == SY_RESCODE_OK)
	{
		SY_LOG_MONO(SY_LOGLEVEL_DEBUG, "destroy done");
	}
	else
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "destroy failed; %s", r.message.ToString());
		ForceDestroy();
	}
}

void SyMono::HotReload()
{
	SY_LOG_MONO(SY_LOGLEVEL_INFO, "hot reload start..");

	SyResult r = HotReloadImpl();
	if (r.code == SY_RESCODE_OK)
	{
		SY_LOG_MONO(SY_LOGLEVEL_INFO, "hot reload done");
	}
	else
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "hot reload failed; %s", r.message.ToString().c_str());
		ForceDestroy();
	}
}

SyMonoGame* SyMono::GetGame()
{
	return &_game;
}

SyResult SyMono::InitImpl()
{
	SY_RESULT_CHECK_EXT(_runtime.Init(), "runtime");
	return {};
}

SyResult SyMono::DestroyImpl()
{
	SY_RESULT_CHECK_EXT(_game.Destroy(), "game");
	SY_RESULT_CHECK_EXT(_gameConfig.Destroy(), "game config");
	return {};
}

SyResult SyMono::HotReloadImpl()
{
	SY_RESULT_CHECK_EXT(_game.Destroy(), "game destroy");
	SY_RESULT_CHECK_EXT(_gameConfig.Destroy(), "game config destroy");
	SY_RESULT_CHECK_EXT(_logger.Destroy(), "logger destroy");

	SY_RESULT_CHECK_EXT(_runtime.Reload(), "runtime reload");

	SY_RESULT_CHECK_EXT(_logger.Create(_runtime), "logger create");
	SY_RESULT_CHECK_EXT(_gameConfig.Create(_runtime), "game config create");
	SY_RESULT_CHECK_EXT(_game.Create(_runtime), "game create");

	_game.EG_SetConfig.Invoke(_gameConfig.GetInstance());

	return {};
}

void SyMono::ForceDestroy()
{
	_runtime.Destroy();

	_logger = {};
	_game = {};
	_gameConfig = {};

	SY_LOG_MONO(SY_LOGLEVEL_WARNING, "force destroy");
}
