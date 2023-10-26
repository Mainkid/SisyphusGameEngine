#pragma once
#include "../Core/IService.h"
#include "SyMonoRuntime.h"
#include "Objects/SyMonoGame.h"
#include "Objects/SyMonoGameConfig.h"
#include "Objects/SyMonoLogger.h"

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

		SyMonoLogger _logger;
		SyMonoGame _game;
		SyMonoGameConfig _gameConfig;

		SyResult InitImpl();
		SyResult DestroyImpl();
		SyResult HotReloadImpl();

		void ForceDestroy();
	};
}
