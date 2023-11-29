#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"
#include "../Components/CameraComponent.h"

struct TransformComponent;
struct EngineContext;
struct HardwareContext;


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
	std::hash<CameraComponent> _hasher;

	EngineContext* _ec = nullptr;
	HardwareContext* _hc = nullptr;

	Vector4 offset = Vector4(3, 3, 3, 0);
	bool _isFlying = false;
	Vector4 targetPosition = Vector4::Zero;
	Vector4 startPosition = Vector4::Zero;
	Quaternion startRotation = Quaternion::Identity;
	float _flyingTime = 0.0f;
	float _cameraArm = 5.0f;
	float _flyingTimeMax = 0.5f;
};

