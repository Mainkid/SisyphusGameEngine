#pragma once
#include "ISystem.h"
#include "SimpleMath.h"
#include "../Components/TransformComponent.h"

struct EngineContext;

using namespace DirectX::SimpleMath;
class TransformSystem: public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

private:
	std::hash<TransformComponent> hasher;
	EngineContext* ec;
};

