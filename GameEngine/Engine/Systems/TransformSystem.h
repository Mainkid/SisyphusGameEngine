#pragma once
#include "ISystem.h"
#include "SimpleMath.h"
#include "../Core/EngineCore.h"

using namespace DirectX::SimpleMath;
class TransformSystem: public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;

private:
	std::hash<TransformComponent> hasher;
};

