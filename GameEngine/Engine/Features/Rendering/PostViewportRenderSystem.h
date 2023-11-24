#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"
#include <memory>

struct HardwareContext;
class PostViewportRenderSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	HardwareContext* hc;
};

