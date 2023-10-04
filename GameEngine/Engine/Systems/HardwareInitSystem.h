#pragma once
#include "../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"
#include "HardwareContext.h"
#include <memory>

class EngineCore;

class HardwareInitSystem : public SystemBase
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	void CreateDeviceAndSwapChain();
	void InitializeDirectX();
	void CreateOSWindow(LPCWSTR appName, HINSTANCE hInstance, const int& width, const int& height);
	EngineCore* engine;
	HardwareContext* hc;

};

