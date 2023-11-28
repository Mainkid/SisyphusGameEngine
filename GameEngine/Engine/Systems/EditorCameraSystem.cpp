#include "EditorCameraSystem.h"
#include "../Contexts/HardwareContext.h"
#include "../Contexts/EngineContext.h"
#include "../Components/CameraComponent.h"
#include "../Components/TransformComponent.h"
#include "../Core/ServiceLocator.h"
#include "../../../vendor/ImGui/imgui.h"
#include "../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../vendor/ImGui/imgui_impl_win32.h"
#include "../Events/SyEditorCameraMoveToTarget.h"
#include "../Scene/CameraHelper.h"
#include "../Scene/GameObjectHelper.h"

SyResult EditorCameraSystem::Init()
{
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	_hc = ServiceLocator::instance()->Get<HardwareContext>();
	auto id = _ecs->create();
	auto& tc = _ecs->emplace<TransformComponent>(id);
	tc.localPosition = Vector3(-3, 3, -3);
	tc._position = Vector3(-3, 3, -3);
	CameraComponent& cc = _ecs->emplace<CameraComponent>(id);
	SetLookAtPos(Vector3(0,0,0), tc);
	return SyResult();
}

SyResult EditorCameraSystem::Run()
{
	auto [cameraComp, cameraTc] = CameraHelper::Find(_ecs);
	auto eventView = _ecs->view<SyEditorCameraMoveToTarget>();
	for (auto& entEvent : eventView)
	{
		auto& ent = _ecs->get<SyEditorCameraMoveToTarget>(entEvent);
		auto& tc=_ecs->get<TransformComponent>(ent.targetEntity);
		
		targetPosition = Vector4(tc._position.x,tc._position.y,tc._position.z,1);
		startPosition = Vector4(cameraTc.localPosition.x, cameraTc.localPosition.y, cameraTc.localPosition.z,1);
		startRotation = Quaternion::CreateFromYawPitchRoll(cameraTc.localRotation);
		
		_isFlying = true;
	}

	auto view = _ecs->view<TransformComponent, CameraComponent>();
	for (auto& entity : view)
	{
		TransformComponent& tc = view.get<TransformComponent>(entity);
		CameraComponent& cc = view.get<CameraComponent>(entity);

		if (tc.worldHash != cc.transformHash)
		{
			cc.transformHash = tc.worldHash;
			UpdateViewMatrix(cc, tc);
		}
		uint32_t hash = _hasher(cc);
		if (hash != cc.hash)
		{
			cc.hash = hash;
			UpdateProjectionMatrix(cc);
		}
		ProcessInput(cc, tc);
	}
	

	if (_isFlying )
	{
		_flyingTime += _ec->deltaTime;
		
		/*auto lookAt = Vector3(offset / offset.Length());
		auto targetRot=Quaternion::FromToRotation(lookAt, Vector3(cameraComp.forward));
		Vector3 rots=Quaternion::Lerp(startRotation, targetRot, _flyingTime / 0.5f).ToEuler();
		cameraTc.localRotation = rots;*/
		cameraTc.localPosition=Vector3(Vector4::Lerp(startPosition, targetPosition+offset, _flyingTime/0.5f));
		//SetLookAtPos(Vector3::Lerp(Vector3(startForwardVector), -Vector3(targetPosition + offset), _flyingTime / 0.5f),cameraTc);
		if (_flyingTime>0.5f)
		{
			_isFlying = false;
			_flyingTime = 0;
		}
	}

	return SyResult();
}

SyResult EditorCameraSystem::Destroy()
{
	return SyResult();
}

void EditorCameraSystem::UpdateViewMatrix(CameraComponent& cc, TransformComponent& tc)
{
	Matrix camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
		tc.localRotation.x, tc.localRotation.y, tc.localRotation.z);

	Vector3 camTarget = DirectX::XMVector3TransformCoord(
		cc.FORWARD_VECTOR, camRotationMatrix);

	camTarget += tc.localPosition;

	DirectX::XMVECTOR upDir = DirectX::XMVector3TransformCoord(cc.UP_VECTOR, camRotationMatrix);
	cc.view = DirectX::XMMatrixLookAtLH(tc.localPosition, camTarget, upDir);
	cc.forward = Vector4::Transform(cc.FORWARD_VECTOR, camRotationMatrix);
	cc.up = Vector4::Transform(cc.UP_VECTOR, camRotationMatrix);
	cc.back = Vector4::Transform(cc.BACKWARD_VECTOR, camRotationMatrix);
	cc.left = Vector4::Transform(cc.LEFT_VECTOR, camRotationMatrix);
	cc.right = Vector4::Transform(cc.RIGHT_VECTOR, camRotationMatrix);
}

void EditorCameraSystem::UpdateProjectionMatrix(CameraComponent& cc)
{
	float fovRadians = (cc.fovDegrees / 360.0f) * DirectX::XM_2PI;
	cc.projection = DirectX::XMMatrixPerspectiveFovLH(fovRadians, cc.aspectRatio, cc.nearPlane, cc.farPlane);
}

void EditorCameraSystem::SetLookAtPos(Vector3 lookAtPos, TransformComponent& tc)
{
	if (lookAtPos.x == tc.localPosition.x && lookAtPos.y == tc.localPosition.y && lookAtPos.z == tc.localPosition.z)
		return;

	lookAtPos.x = -lookAtPos.x + tc.localPosition.x;
	lookAtPos.y = -lookAtPos.y + tc.localPosition.y;
	lookAtPos.z = -lookAtPos.z + tc.localPosition.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}

	if (lookAtPos.z > 0)
		yaw += DirectX::XM_PI;
	tc.localRotation.x = pitch;
	tc.localRotation.y = yaw;
}

void EditorCameraSystem::ProcessInput(CameraComponent& cc, TransformComponent& tc)
{
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
	{
		tc.localRotation += SyVector3(io.MouseDelta.y * _ec->deltaTime, io.MouseDelta.x * _ec->deltaTime, 0.0f);
	}
	if (ImGui::IsKeyDown(ImGuiKey_W))
	{
		tc.localPosition += Vector3(cc.forward) * cc.cameraSpeed * _ec->deltaTime;
	}
	if (ImGui::IsKeyDown(ImGuiKey_A))
	{
		tc.localPosition += Vector3(cc.left) * cc.cameraSpeed * _ec->deltaTime;
	}
	if (ImGui::IsKeyDown(ImGuiKey_S))
	{
		tc.localPosition += Vector3(cc.back) * cc.cameraSpeed * _ec->deltaTime;
	}
	if (ImGui::IsKeyDown(ImGuiKey_D))
	{
		tc.localPosition += Vector3(cc.right) * cc.cameraSpeed * _ec->deltaTime;
	}
	if (ImGui::IsKeyDown(ImGuiKey_Space))
	{
		tc.localPosition += Vector3(0, cc.cameraSpeed * _ec->deltaTime, 0.0f);
	}
	if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
	{
		tc.localPosition += Vector3(0, -cc.cameraSpeed * _ec->deltaTime, 0.0f);
	}
	tc.localPosition += Vector3(cc.forward) * cc.mouseWheel * 0.25f;
	cc.mouseWheel = 0.0f;
	//std::cout << io.MouseWheel << std::endl;
}
