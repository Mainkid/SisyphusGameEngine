#include "MonoGameLoopSystem.h"

#include "../SyMono.h"
#include "../../Core/ServiceLocator.h"


SyResult MonoGameLoopSystem::Init()
{
	auto mono = ServiceLocator::instance()->Get<mono::SyMono>();
	_monoEcs = mono->GetEcs();
	_monoGame = mono->GetGame();

	_testTimeOnPrevFrame = std::chrono::steady_clock::now();

	return {};
}

SyResult MonoGameLoopSystem::Run()
{
	OPTICK_EVENT()

	if (!_monoEcs->IsValid() || !_monoGame->IsValid())
		return {};

	if (true)
	{
		//TODO: rewrite when engine-context-time will be fixed.
		auto time = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(time - _testTimeOnPrevFrame).count() / 1000000.0f;
		_testTotalTime += deltaTime;
		_testTimeOnPrevFrame = time;

		mono::ProxyTimeData timeData;
		timeData.TotalTime = _testTotalTime;
		timeData.DeltaTime = deltaTime;

		_monoGame->EgLoopRun.Invoke(timeData);
	}

	_monoEcs->EgSyncEngineWithGame.Invoke();

	return {};
}

SyResult MonoGameLoopSystem::Destroy()
{
	if (_monoGame->IsValid())
		_monoGame->EgLoopDestroy.Invoke();

	return {};
}