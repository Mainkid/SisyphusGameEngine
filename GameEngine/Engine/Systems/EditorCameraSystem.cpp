#include "EditorCameraSystem.h"
#include "../Components/CameraComponent.h"

void EditorCameraSystem::Init()
{
    auto id = GetScene()->registry.create();
    GetScene()->registry.emplace<DataComponent>(id,"Camera");
    GetScene()->registry.emplace<TransformComponent>(id);
    CameraComponent& cc = GetScene()->registry.emplace<CameraComponent>(id);
    GetScene()->camera = &cc;
}

void EditorCameraSystem::Run()
{
	auto view = EngineCore::instance()->scene->registry.view<TransformComponent,CameraComponent>();
	for (auto& entity : view)
	{
		TransformComponent& tc = view.get<TransformComponent>(entity);
		CameraComponent& cc = view.get<CameraComponent>(entity);
		if (tc.hash != cc.transformHash)
		{
			tc.hash = cc.transformHash;
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

void EditorCameraSystem::ProcessInput(CameraComponent& cc, TransformComponent& tc)
{
    if (EngineCore::instance()->hud->GetFocusedWindowName() == "Viewport")
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
        {
            tc.rotation += Vector3(io.MouseDelta.y * EngineCore::instance()->deltaTime, io.MouseDelta.x * EngineCore::instance()->deltaTime, 0.0f);
        }
        //this->camera->AdjustRotation(io.MouseDelta.y * EngineCore::instance()->deltaTime, io.MouseDelta.x * EngineCore::instance()->deltaTime, 0.0f);

        //EngineCore::instance()->H
        if (ImGui::IsKeyDown(ImGuiKey_W))
        {
            tc.position += Vector3(cc.forward) * cc.cameraSpeed * EngineCore::instance()->deltaTime;
            //this->camera->AdjustPosition(cc.forward * cc.cameraSpeed * EngineCore::instance()->deltaTime);

        }
        if (ImGui::IsKeyDown(ImGuiKey_A))
        {
            tc.position += Vector3(cc.left) * cc.cameraSpeed * EngineCore::instance()->deltaTime;
            //this->camera->AdjustPosition(this->camera->GetLeftVector() * cameraSpeed * deltaSec);

        }
        if (ImGui::IsKeyDown(ImGuiKey_S))
        {
            tc.position += Vector3(cc.back) * cc.cameraSpeed * EngineCore::instance()->deltaTime;
            //this->camera->AdjustPosition(this->camera->GetBackwardVector() * cameraSpeed * deltaSec);
        }
        if (ImGui::IsKeyDown(ImGuiKey_D))
        {
            tc.position += Vector3(cc.right) * cc.cameraSpeed * EngineCore::instance()->deltaTime;
           // this->camera->AdjustPosition(this->camera->GetRightVector() * cameraSpeed * deltaSec);
        }
        if (ImGui::IsKeyDown(ImGuiKey_Space))
        {
            tc.position += Vector3(0, cc.cameraSpeed* EngineCore::instance()->deltaTime,0.0f);

            //this->camera->AdjustPosition(0.0f, cameraSpeed * deltaSec, 0.0f);
        }
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
        {
            tc.position += Vector3(0, -cc.cameraSpeed * EngineCore::instance()->deltaTime, 0.0f);
                      
        }
        if (ImGui::IsKeyDown(ImGuiKey_E))
        {
           
        }
    }
}
