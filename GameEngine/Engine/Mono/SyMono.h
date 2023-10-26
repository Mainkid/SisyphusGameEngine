#pragma once
#include "SyMonoGame.h"
#include "SyMonoRuntime.h"
#include "../Core/IService.h"

namespace mono
{
	class SyMono : public IService
	{
	public:
		~SyMono();

		void Init();
		void Destroy();

		void HotReload();

		SyMonoGame* GetGame();

	private:
		SyMonoRuntime _runtime;

		SyMonoGame _game;
		SyMonoGameConfig _gameConfig;
	};
}
