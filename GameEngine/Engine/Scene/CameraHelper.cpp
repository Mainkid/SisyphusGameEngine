#include "CameraHelper.h"

std::tuple<CameraComponent&, TransformComponent&> CameraHelper::Find(entt::registry* ecs)
{
	auto view = ecs->view<CameraComponent, TransformComponent>();
	return view.get(view.front());
}