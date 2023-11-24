#pragma once
#include "../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"
#include "../Contexts/HardwareContext.h"
#include <memory>

class EngineCore;

class HardwareInitSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	void CreateDeviceAndSwapChain();
	void InitializeDirectX();
	void CreateOSWindow(LPCWSTR appName, HINSTANCE hInstance, const int& width, const int& height);
	EngineCore* engine;
	HardwareContext* hc;

};

