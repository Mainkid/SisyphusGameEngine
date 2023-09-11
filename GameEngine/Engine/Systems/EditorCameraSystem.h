#pragma once
#include "ISystem.h"
#include "SimpleMath.h"
#include "../Components/CameraComponent.h"
#include "../Core/EngineCore.h"


class EditorCameraSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	void UpdateViewMatrix(CameraComponent& cc,TransformComponent& tc);
	void UpdateProjectionMatrix(CameraComponent& cc);
	void SetLookAtPos(Vector3 pos, TransformComponent& tc);
	void ProcessInput(CameraComponent& cc, TransformComponent& tc);
	std::hash<CameraComponent> hasher;
};

