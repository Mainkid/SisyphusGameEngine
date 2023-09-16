#pragma once
#include "../Core/Scene/Scene.h"
#include "../Core/IService.h"

struct EngineContext : public IService
{
	std::unique_ptr<Scene> scene;
	float totalTime = 0;
	unsigned int frameCount = 0;
	float deltaTime = 0;
};
