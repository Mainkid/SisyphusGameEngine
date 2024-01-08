#pragma once

#include "../../Core/ECS/SystemBase.h"

struct EngineContext;

namespace mono
{
	class SyMonoEcs;
	class SyMonoGame;
}

class MonoGameLoopSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

private:
	mono::SyMonoEcs* _monoEcs = nullptr;
	mono::SyMonoGame* _monoGame = nullptr;

	EngineContext* _engineContext = nullptr;

	std::chrono::time_point<std::chrono::steady_clock> _testTimeOnPrevFrame;
	float _testTotalTime = 0;
};
