#pragma once
#include "ISystem.h"
#include "../Components/Material.h"

struct EngineContext;
struct RenderContext;
struct HardwareContext;

class MaterialUpdateSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;

	std::hash<Material> hasher;
};

