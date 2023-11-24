#pragma once
#include "../Resources/ResourceService.h"
#include "../../Core/ECS/SystemBase.h"

struct ImageBasedLightingComponent;
struct RenderContext;
struct EngineContext;
struct HardwareContext;

class ImageBasedLightingSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	void InitializeResources(ImageBasedLightingComponent& iblComp);
	float CreateFilteredCubeMap(ImageBasedLightingComponent& iblComp,int resolution);
	RenderContext* _rc = nullptr;
	EngineContext* _ec = nullptr;
	HardwareContext* _hc = nullptr;
	ResourceService* _rs = nullptr;

	const int _irradianceResolution = 32;
	const int _lookUpTextureResolution = 512;
};

