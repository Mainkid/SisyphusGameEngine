#pragma once
#include <tuple>

#include "../Components/CameraComponent.h"
#include "../Components/TransformComponent.h"
#include "../Contexts/EngineContext.h"

class CameraHelper
{
public:
	static std::tuple<CameraComponent&, TransformComponent&> Find(entt::registry* ecs,EngineContext::EPlayModeState playModeState);
};
