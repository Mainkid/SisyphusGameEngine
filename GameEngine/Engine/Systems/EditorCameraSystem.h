#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"
#include "../Components/CameraComponent.h"

struct TransformComponent;
struct EngineContext;
struct HardwareContext;

using namespace DirectX::SimpleMath;
class EditorCameraSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	void UpdateViewMatrix(CameraComponent& cc,TransformComponent& tc);
	void UpdateProjectionMatrix(CameraComponent& cc);
	void SetLookAtPos(Vector3 pos, TransformComponent& tc);
	void ProcessInput(CameraComponent& cc, TransformComponent& tc);
	std::hash<CameraComponent> hasher;

	EngineContext* ec;
	HardwareContext* hc;
};

