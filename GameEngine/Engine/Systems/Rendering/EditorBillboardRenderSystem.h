#pragma once
#include "../ISystem.h"
#include "../../Core/ServiceLocator.h"
#include "RenderContext.h"
#include <memory>

class EngineCore;

class EditorBillboardRenderSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	EngineCore* engine;
	RenderContext* rc;
};

