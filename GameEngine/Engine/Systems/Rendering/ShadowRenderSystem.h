#pragma once
#include "../ISystem.h"
#include "RenderContext.h"

class EngineCore;


class ShadowRenderSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	EngineCore* engine;
	RenderContext* rc;

};