#pragma once
#include "../../Core/ECS/SystemBase.h"

namespace mono
{
	class SyMono;
}

class MonoHotReloadSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

private:
	mono::SyMono* _mono;
};
