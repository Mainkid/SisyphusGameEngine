#include "CameraHelper.h"

std::tuple<CameraComponent&, TransformComponent&> CameraHelper::Find(entt::registry* ecs, EngineContext::EPlayModeState playModeState)
{
	auto view = ecs->view<CameraComponent, TransformComponent>();

	for (auto& ent : view)
	{
		auto [cameraComp, transformComp] = view.get(ent);
		if (playModeState != EngineContext::EPlayModeState::EditorMode &&
			cameraComp.cameraType == ECameraType::PlayerCamera)
		{
			return view.get(ent);
		}
		else if (playModeState == EngineContext::EPlayModeState::EditorMode &&
			cameraComp.cameraType == ECameraType::EditorCamera)
		{
			return view.get(ent);
		}
	}

	return view.get(view.front());
}