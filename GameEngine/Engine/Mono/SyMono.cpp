#include "SyMono.h"


using namespace mono;

SyMono::~SyMono()
{
	Destroy();
}

void SyMono::Init()
{
	_runtime.Init();
}

void SyMono::Destroy()
{
	_game.Destroy();
	_gameConfig.Destroy();

	_runtime.Destroy();
}

void SyMono::HotReload()
{
	_game.Destroy();
	_gameConfig.Destroy();

	_runtime.Reload();

	_game.Create(_runtime);
	_gameConfig.Create(_runtime);

	_game.EG_SetConfig.Invoke(_gameConfig.GetInstance());
}

SyMonoGame* SyMono::GetGame()
{
	return &_game;
}
