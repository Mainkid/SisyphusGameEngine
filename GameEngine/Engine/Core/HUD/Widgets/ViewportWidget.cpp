#pragma once
#include "../../../Systems/Rendering/RenderHelper.h"
#include "../../../Core/Rendering/RenderTarget.h"
#include "../../../Systems/Rendering/RenderHelper.h"
#include "../../ServiceLocator.h"
#include "../../../Systems/HardwareContext.h"
#include "../Hud.h"
#include "ViewportWidget.h"
#include "../../../../vendor/Delegates.h"
#include "../../../Systems/EngineContext.h"
#include "../../../Core/ServiceLocator.h"


ViewportWidget::ViewportWidget(Hud* _hud)
{
	this->hud = _hud;
	this->windowID = "Viewport";
	ec = ServiceLocator::instance()->Get<EngineContext>();
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	InitSRV();
}

void ViewportWidget::InitSRV()
{
	int width, height;
	ImageLoader::LoadTextureFromFile(translateIconPath.string().c_str(),translateSRV.GetAddressOf(), &width, &height);
	guizmoIconSRV[ImGuizmo::OPERATION::TRANSLATE] = translateSRV.Get();

	ImageLoader::LoadTextureFromFile(rotateIconPath.string().c_str(), rotateSRV.GetAddressOf(), &width, &height);
	guizmoIconSRV[ImGuizmo::OPERATION::ROTATE] = rotateSRV.Get();

	ImageLoader::LoadTextureFromFile(scaleIconPath.string().c_str(), scaleSRV.GetAddressOf(), &width, &height);
	guizmoIconSRV[ImGuizmo::OPERATION::SCALE] =scaleSRV.Get();
}

using namespace DirectX::SimpleMath;
void ViewportWidget::Render()
{
	ImGui::Begin(windowID.c_str());

	Widget::Render();

	ImVec2 imgSize = ImGui::GetContentRegionAvail();
	ImVec2 startCursorPos = ImGui::GetCursorScreenPos();
	//ImGui::SameLine();
	//ImGui::ImageButton((void*)EngineCore::instance()->renderTarget->GetImageSRV(), { 20,20 });
	//ImGui::ImageButton((void*)EngineCore::instance()->renderTarget->GetImageSRV(), { 20,20 });

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::Image((void*)hc->renderTarget->GetImageSRV(), ImGui::GetContentRegionAvail());
	ImVec2 r = ImGui::GetItemRectSize();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left)&& !ImGuizmo::IsOver()&& hoverState==EHoveringState::Viewport)
	{
		
		ImVec2 pos2 = ImGui::GetMousePos();
		ImVec2 pos = ImGui::GetCursorScreenPos();
		//std::cout << "Is over" << std::endl;
		auto textureSize = RenderHelper::GetRtvResolution();


		pos = ImVec2(pos2.x- pos.x, pos2.y - pos.y+imgSize.y);
	    x = std::clamp((int)(pos.x * textureSize.x / imgSize.x),0, (int)textureSize.x-1);
		y = std::clamp((int)(pos.y * textureSize.y / imgSize.y),0, (int)textureSize.y-1);
		
		int pixelColorR =RenderHelper::GetPixelValue(x, y);
		if (pixelColorR != -1)
		{
			hud->UpdateSelectedEntityEvent.Broadcast(entt::entity(pixelColorR));
		}

	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ec->scene->camera->mouseWheel = io.MouseWheel;

	//Gizmos
	if (hud->selectedEntityID!=entt::null)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		
		auto viewMat = ec->scene->camera->view;
		auto projMat = ec->scene->camera->projection;
		auto& tc = ec->scene->registry.get<TransformComponent>(hud->selectedEntityID);
		auto transformMat = tc.transformMatrix;
		//auto transformMat = TransformHelper::ConstructTransformMatrix(tc);

		auto res=ImGuizmo::Manipulate(&viewMat._11, &projMat._11, guizmoType, ImGuizmo::LOCAL, &transformMat._11);
		
		if (ImGuizmo::IsUsing())	
		{
			Vector3 translation, scale,translationL,scaleL;
			Quaternion rotation, rotationL;
			transformMat = transformMat * TransformHelper::ConstructInverseParentTransform(tc);
			transformMat.Decompose(scale, rotation, translation);
			Vector3 deltaRotation = rotation.ToEuler() - tc.localRotation;
			tc.localRotation = tc.localRotation + deltaRotation;
			tc.localPosition = translation;
			tc.localScale = scale;
			//TransformHelper::RecalculateWorldPos(tc);
		}
	}

	if (ImGui::IsKeyDown(ImGuiKey_R))
		guizmoType = ImGuizmo::OPERATION::ROTATE;
	if (ImGui::IsKeyDown(ImGuiKey_T))
		guizmoType = ImGuizmo::OPERATION::TRANSLATE;
	if (ImGui::IsKeyDown(ImGuiKey_E))
		guizmoType = ImGuizmo::OPERATION::SCALE;


	hoverState = EHoveringState::Viewport;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	ImGui::SetNextItemAllowOverlap();
	ImGui::SetCursorScreenPos(startCursorPos);
	ImGui::ImageButton((void*)guizmoIconSRV[ImGuizmo::OPERATION::TRANSLATE], { 20,20 });
	if (ImGui::IsItemHovered())
	{
		hoverState = EHoveringState::TransformGizmo;
		
	}
	if (ImGui::IsItemClicked())
	{
		guizmoType = ImGuizmo::OPERATION::TRANSLATE;
	}

	ImGui::SameLine();
	ImGui::SetNextItemAllowOverlap();
	ImGui::ImageButton((void*)guizmoIconSRV[ImGuizmo::OPERATION::ROTATE], { 20,20 });
	if (ImGui::IsItemHovered())
	{
		hoverState = EHoveringState::RotateGizmo;

	}
	if (ImGui::IsItemClicked())
	{
		guizmoType = ImGuizmo::OPERATION::ROTATE;
	}

	ImGui::SameLine();
	ImGui::SetNextItemAllowOverlap();
	ImGui::ImageButton((void*)guizmoIconSRV[ImGuizmo::OPERATION::SCALE], { 20,20 });
	if (ImGui::IsItemHovered())
	{
		hoverState = EHoveringState::ScaleGizmo;

	}
	if (ImGui::IsItemClicked())
	{
		guizmoType = ImGuizmo::OPERATION::SCALE;
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	//std::cout <<(int) hoverState << std::endl;

	HandleResize();
	ImGui::End();
}

void ViewportWidget::HandleResize()
{
	auto newWidgetSize = ImGui::GetWindowSize();
	if (widgetSize.x != newWidgetSize.x || widgetSize.y != newWidgetSize.y)
	{
		hud->ViewportResizedEvent.Broadcast(newWidgetSize.x * 1.0f / newWidgetSize.y);
		widgetSize = newWidgetSize;

		auto view = ec->scene->registry.view<TransformComponent, CameraComponent>();
		for (auto& entity : view)
		{
			CameraComponent& cc = view.get<CameraComponent>(entity);
			cc.aspectRatio = widgetSize.x / widgetSize.y;
		}
	}
}

void ViewportWidget::GetInput()
{
	
}

void ViewportWidget::UpdateSelectedEntity(entt::entity)
{
	
}
