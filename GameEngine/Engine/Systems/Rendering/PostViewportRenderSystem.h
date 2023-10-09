#pragma once
#include "../ISystem.h"
#include "../../Core/ServiceLocator.h"
#include <memory>

struct HardwareContext;
class PostViewportRenderSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	HardwareContext* hc;
};

