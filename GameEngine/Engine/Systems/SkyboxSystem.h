#pragma once
#include "../Core/ECS/SystemBase.h"

struct HardwareContext;
struct EngineContext;
class ResourceService;

class SkyboxSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	HardwareContext* _hc = nullptr;
	EngineContext* _ec = nullptr;
	ResourceService* _rs = nullptr;
};

