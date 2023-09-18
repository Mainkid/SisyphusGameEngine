#pragma once
#include "../ISystem.h"
#include "../../Core/ServiceLocator.h"
#include <memory>

struct HardwareContext;
class PostViewportRenderSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	HardwareContext* hc;
};

