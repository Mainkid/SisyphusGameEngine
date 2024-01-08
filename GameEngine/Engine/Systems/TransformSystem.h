#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"
#include "../Components/TransformComponent.h"

struct EngineContext;



using namespace DirectX::SimpleMath;
class TransformSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

	enum TransformType
	{
		LOCAL,
		WORLD,
		CODE
	};

private:
	void CheckTransform(TransformComponent& tc);
	TransformType transformType = LOCAL;
	EngineContext* ec;
};
