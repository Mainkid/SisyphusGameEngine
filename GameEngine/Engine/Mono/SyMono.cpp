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

SyMonoEcs* SyMono::GetEcs()
{
	return &_ecs;
}

SyMonoEditor* SyMono::GetEditor()
{
	return &_editor;
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
	SY_RESULT_CHECK_EXT(_gameConfig.Destroy(), "game config");
	SY_RESULT_CHECK_EXT(_game.Destroy(), "game");
	SY_RESULT_CHECK_EXT(_editor.Destroy(), "editor");
	SY_RESULT_CHECK_EXT(_ecs.Destroy(), "ecs");
	SY_RESULT_CHECK_EXT(_logger.Destroy(), "logger");
	return {};
}

SyResult SyMono::HotReloadImpl()
{
	SY_RESULT_CHECK_EXT(DestroyImpl(), "destroy");

	SY_RESULT_CHECK_EXT(_runtime.Reload(), "runtime reload");

	SY_RESULT_CHECK_EXT(_logger.Create(_runtime), "logger create");
	SY_RESULT_CHECK_EXT(_ecs.Create(_runtime), "ecs create");
	SY_RESULT_CHECK_EXT(_editor.Create(_runtime), "editor create");
	SY_RESULT_CHECK_EXT(_game.Create(_runtime), "game create");
	SY_RESULT_CHECK_EXT(_gameConfig.Create(_runtime), "game config create");

	_editor.EgInit.Invoke(_ecs.GetInstance());
	_game.EgInit.Invoke(_ecs.GetInstance(), _gameConfig.GetInstance());

	return {};
}

void SyMono::ForceDestroy()
{
	_runtime.Destroy();

	_gameConfig = {};
	_game = {};
	_editor = {};
	_ecs = {};
	_logger = {};

	SY_LOG_MONO(SY_LOGLEVEL_WARNING, "force destroy");
}
