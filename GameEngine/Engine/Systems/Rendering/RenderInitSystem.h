#pragma once
#include "../ISystem.h"
#include "../../Core/ServiceLocator.h"
#include "../../Core/EngineCore.h"
#include "RenderContext.h"
#include <memory>

class RenderInitSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;

};

