#include "SyMono.h"


using namespace mono;

SyMono::~SyMono()
{
	Destroy();
}

void SyMono::Init()
{
	SY_LOG_MONO(SY_LOGLEVEL_INFO, "init start..");

	SyResult r = _runtime.Init();

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

	SyResult r = DestroyObjects();
	if (r.code == SY_RESCODE_OK)
	{
		_runtime.Destroy();
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
	SyResult r{};

	SY_LOG_MONO(SY_LOGLEVEL_INFO, "[hot reload] destroying old objects..");
	r = DestroyObjects();
	if (r.code != SY_RESCODE_OK)
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "[hot reload] failed; %s", r.message.ToString().c_str());
		return;
	}
	SY_LOG_MONO(SY_LOGLEVEL_INFO, "[hot reload] old objects are destroyed");

	SY_LOG_MONO(SY_LOGLEVEL_INFO, "[hot reload] reloading runtime assembly..");
	r = _runtime.Reload();
	if (r.code != SY_RESCODE_OK)
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "[hot reload] failed; %s", r.message.ToString().c_str());
		return;
	}
	SY_LOG_MONO(SY_LOGLEVEL_INFO, "[hot reload] runtime assembly is reloaded");

	SY_LOG_MONO(SY_LOGLEVEL_INFO, "[hot reload] creating new objects..");
	r = CreateObjects();
	if (r.code != SY_RESCODE_OK)
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "[hot reload] failed; %s", r.message.ToString().c_str());
		return;
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


SyResult SyMono::CreateObjects()
{
	SY_RESULT_CHECK_EXT(_logger.Create(_runtime), "logger create");
	SY_RESULT_CHECK_EXT(_ecs.Create(_runtime), "ecs create");
	SY_RESULT_CHECK_EXT(_editor.Create(_runtime), "editor create");
	SY_RESULT_CHECK_EXT(_input.Create(_runtime), "input create");
	SY_RESULT_CHECK_EXT(_game.Create(_runtime), "game create");
	SY_RESULT_CHECK_EXT(_gameConfig.Create(_runtime), "game config create");

	_editor.EgInit.Invoke(_ecs.GetInstance());
	_game.EgInit.Invoke(_ecs.GetInstance(), _input.GetInstance(), _gameConfig.GetInstance());

	return {};
}

SyResult SyMono::DestroyObjects()
{
	SY_RESULT_CHECK_EXT(_gameConfig.Destroy(), "game config destroy");
	SY_RESULT_CHECK_EXT(_game.Destroy(), "game destroy");
	SY_RESULT_CHECK_EXT(_input.Destroy(), "input destroy");
	SY_RESULT_CHECK_EXT(_editor.Destroy(), "editor destroy");
	SY_RESULT_CHECK_EXT(_ecs.Destroy(), "ecs destroy");
	SY_RESULT_CHECK_EXT(_logger.Destroy(), "logger destroy");
	return {};
}


void SyMono::ForceDestroy()
{
	_runtime.Destroy();

	_gameConfig = {};
	_game = {};
	_input = {};
	_editor = {};
	_ecs = {};
	_logger = {};

	SY_LOG_MONO(SY_LOGLEVEL_WARNING, "force destroy");
}
