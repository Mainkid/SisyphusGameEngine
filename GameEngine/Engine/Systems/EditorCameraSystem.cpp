#include "EditorCameraSystem.h"
#include "HardwareContext.h"
#include "EngineContext.h"
#include "../Components/CameraComponent.h"
#include "../Components/TransformComponent.h"
#include "../Core/ServiceLocator.h"
#include "../../../vendor/ImGui/imgui.h"
#include "../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../vendor/ImGui/imgui_impl_win32.h"

void EditorCameraSystem::Init()
{
    ec = ServiceLocator::instance()->Get<EngineContext>();
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    auto id = ec->scene->registry.create();
    ec->scene->registry.emplace<DataComponent>(id,"Camera");
    TransformComponent& tc=ec->scene->registry.emplace<TransformComponent>(id);
    CameraComponent& cc = ec->scene->registry.emplace<CameraComponent>(id);
    ec->scene->camera = &cc;
    ec->scene->cameraTransform = &tc;
    SetLookAtPos(Vector3(-1, 0, 0), tc);
}

void EditorCameraSystem::Run()
{
	auto view = ec->scene->registry.view<TransformComponent,CameraComponent>();
	for (auto& entity : view)
	{
		TransformComponent& tc = view.get<TransformComponent>(entity);
		CameraComponent& cc = view.get<CameraComponent>(entity);
        
		if (tc.hash != cc.transformHash)
		{
            cc.transformHash = tc.hash;
			UpdateViewMatrix(cc,tc);
		}
		uint32_t hash = hasher(cc);
		if (hash != cc.hash)
		{
			cc.hash = hash;
			UpdateProjectionMatrix(cc);
		}
        ProcessInput(cc,tc);
	}

    

}

void EditorCameraSystem::Destroy()
{
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

void EditorCameraSystem::SetLookAtPos(Vector3 lookAtPos,TransformComponent& tc)
{
    if (lookAtPos.x == tc.localPosition.x && lookAtPos.y == tc.localPosition.y && lookAtPos.z == tc.localPosition.z)
        return;

    lookAtPos.x = lookAtPos.x - tc.localPosition.x;
    lookAtPos.y = lookAtPos.y - tc.localPosition.y;
    lookAtPos.z = lookAtPos.z - tc.localPosition.z;

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
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
        {
            tc.localRotation += Vector3(io.MouseDelta.y * ec->deltaTime, io.MouseDelta.x * ec->deltaTime, 0.0f);
        }
        if (ImGui::IsKeyDown(ImGuiKey_W))
        {
            tc.localPosition += Vector3(cc.forward) * cc.cameraSpeed * ec->deltaTime;
            
        }
        if (ImGui::IsKeyDown(ImGuiKey_A))
        {
            tc.localPosition += Vector3(cc.left) * cc.cameraSpeed * ec->deltaTime;
            
        }
        if (ImGui::IsKeyDown(ImGuiKey_S))
        {
            tc.localPosition += Vector3(cc.back) * cc.cameraSpeed * ec->deltaTime;
        }
        if (ImGui::IsKeyDown(ImGuiKey_D))
        {
            tc.localPosition += Vector3(cc.right) * cc.cameraSpeed * ec->deltaTime;
        }
        if (ImGui::IsKeyDown(ImGuiKey_Space))
        {
            tc.localPosition += Vector3(0, cc.cameraSpeed*ec->deltaTime,0.0f);

        }
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
        {
            tc.localPosition += Vector3(0, -cc.cameraSpeed * ec->deltaTime, 0.0f);
                      
        }
        tc.localPosition += Vector3(cc.forward) * cc.mouseWheel*0.25f;
        cc.mouseWheel = 0.0f;
        //std::cout << io.MouseWheel << std::endl;
    
}
