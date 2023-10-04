#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"
#include "../Components/TransformComponent.h"

struct EngineContext;

using namespace DirectX::SimpleMath;
class TransformSystem: public SystemBase
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;

private:
	std::hash<TransformComponent> hasher;
	EngineContext* ec;
};

