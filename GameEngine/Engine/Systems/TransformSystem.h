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
	void UpdateScaleMatrix(TransformComponent& tc, Vector3& scale);
	void UpdateRotationMatrix(TransformComponent& tc, Vector3& rot);
	void UpdateTranslationMatrix(TransformComponent& tc, Vector3& pos);
	std::hash<TransformComponent> hasher;
};

