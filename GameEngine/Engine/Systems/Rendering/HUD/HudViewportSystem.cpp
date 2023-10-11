#include "HudViewportSystem.h"
#include "../../../Core/ServiceLocator.h"
#include "../../ResourceService.h"
#include "../RenderHelper.h"
#include "../../Core/Rendering/RenderTarget.h"
#include "../../EngineContext.h"

SyResult HudViewportSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rs = ServiceLocator::instance()->Get<ResourceService>();
	windowID = "Viewport";
	InitSRV();
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "HudViewport system initialization successful.");
	return SyResult();

}

SyResult HudViewportSystem::Run()
{
	ImGui::Begin(windowID.c_str());

	//Widget::Render();
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

	ImVec2 pos2 = ImGui::GetMousePos();
	ImVec2 pos = ImGui::GetCursorScreenPos();
	pos = ImVec2(pos2.x - pos.x, pos2.y - pos.y + imgSize.y);
	auto textureSize = RenderHelper::GetRtvResolution();
	x = std::clamp((int)(pos.x * textureSize.x / imgSize.x), 0, (int)textureSize.x - 1);
	y = std::clamp((int)(pos.y * textureSize.y / imgSize.y), 0, (int)textureSize.y - 1);
	
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left)&& !ImGuizmo::IsOver()&& hoverState==EHoveringState::Viewport)
	{
		
		int pixelColorR =RenderHelper::GetPixelValue(x, y).x;
		if (pixelColorR != -1)
		{
			ec->selectedEntityID=entt::entity(pixelColorR);
			ec->selectedContent.assetType = EAssetType::ASSET_NONE;
		}

	}

	if (ImGui::BeginDragDropTarget())
	{

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_CONTENTBROWSER"))
		{
			std::string uuidStr;
			uuidStr = static_cast<char*>(payload->Data);
			uuidStr.resize(36);

			auto uuid = boost::lexical_cast<boost::uuids::uuid>(uuidStr);

			if (rs->resourceLibrary[uuid].assetType == EAssetType::ASSET_MESH)
			{
				auto go = ec->scene->AddGameObject();
				auto camera = ec->scene->registry.view<CameraComponent, TransformComponent>();


				ec->scene->registry.get<MeshComponent>(go).modelUUID = uuid;
				auto pos = camera.get<TransformComponent>(camera.front()).localPosition + Vector3(camera.get<CameraComponent>(camera.front()).forward * 5.0f);
				ec->scene->registry.get<TransformComponent>(go).localPosition = pos;
			}
			else if (rs->resourceLibrary[uuid].assetType == EAssetType::ASSET_MATERIAL)
			{
				auto enttID = RenderHelper::GetPixelValue(x, y);
				if (enttID.z == int(EAssetType::ASSET_MESH))
				{
					MeshComponent& meshComp = ec->scene->registry.get<MeshComponent>(static_cast<entt::entity>(enttID.x));
					meshComp.materialUUIDs[enttID.y] = uuid;
					//meshComp.materials[enttID.y] = static_cast<Material*>(rs->LoadResource(uuid));
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ec->scene->camera->mouseWheel = io.MouseWheel;

	//Gizmos
	
	if (ec->selectedEntityID!=entt::null)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		
		auto viewMat = ec->scene->camera->view;
		auto projMat = ec->scene->camera->projection;
		auto& tc = ec->scene->registry.get<TransformComponent>(ec->selectedEntityID);
		auto transformMat = tc.transformMatrix;

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
		}
	}

	if (ImGui::IsKeyDown(ImGuiKey_R))
		guizmoType = ImGuizmo::OPERATION::ROTATE;
	if (ImGui::IsKeyDown(ImGuiKey_T))
		guizmoType = ImGuizmo::OPERATION::TRANSLATE;
	if (ImGui::IsKeyDown(ImGuiKey_E))
		guizmoType = ImGuizmo::OPERATION::SCALE;

	
	drawPlayMode(startCursorPos);
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
	return SyResult();
}

SyResult HudViewportSystem::Destroy()
{

	SY_LOG_CORE(SY_LOGLEVEL_INFO, " HudViewport system destruction successful.");
	return SyResult();
}

void HudViewportSystem::InitSRV()
{
	int width, height;
	ImageLoader::LoadTextureFromFile(translateIconPath.string().c_str(),translateSRV.GetAddressOf(), &width, &height);
	guizmoIconSRV[ImGuizmo::OPERATION::TRANSLATE] = translateSRV.Get();

	ImageLoader::LoadTextureFromFile(rotateIconPath.string().c_str(), rotateSRV.GetAddressOf(), &width, &height);
	guizmoIconSRV[ImGuizmo::OPERATION::ROTATE] = rotateSRV.Get();

	ImageLoader::LoadTextureFromFile(scaleIconPath.string().c_str(), scaleSRV.GetAddressOf(), &width, &height);
	guizmoIconSRV[ImGuizmo::OPERATION::SCALE] =scaleSRV.Get();

	ImageLoader::LoadTextureFromFile(
		playButtonPath.string().c_str(), 
		playBtnSRV.GetAddressOf(), &width, &height
	);

	ImageLoader::LoadTextureFromFile(
		stopButtonPath.string().c_str(),
		stopBtnSRV.GetAddressOf(), &width, &height
	);

	ImageLoader::LoadTextureFromFile(
		pauseButtonPath.string().c_str(),
		pauseBtnSRV.GetAddressOf(), &width, &height
	);
}

void HudViewportSystem::HandleResize()
{
	auto newWidgetSize = ImGui::GetWindowSize();
	if (widgetSize.x != newWidgetSize.x || widgetSize.y != newWidgetSize.y)
	{
		//hud->ViewportResizedEvent.Broadcast(newWidgetSize.x * 1.0f / newWidgetSize.y);
		widgetSize = newWidgetSize;

		auto view = ec->scene->registry.view<TransformComponent, CameraComponent>();
		for (auto& entity : view)
		{
			CameraComponent& cc = view.get<CameraComponent>(entity);
			cc.aspectRatio = widgetSize.x / widgetSize.y;
		}
	}
}

void HudViewportSystem::drawPlayMode(ImVec2 cursorStartPostion)
{
	hoverState = EHoveringState::PlayMode;
	int offset = ImGui::GetContentRegionAvail().x / 2;
	int dtOffset = 24;
	
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	ImGui::SetNextItemAllowOverlap();
	ImGui::SetCursorScreenPos(cursorStartPostion);
	ImGui::SameLine(offset);
	activeButtonBG.w = ec->playModeState == EngineContext::EPlayModeState::PlayMode;
	ImGui::ImageButton((void*)(playBtnSRV.Get()), { 20,20 },ImVec2(0,0),ImVec2(1,1),-1,activeButtonBG);
	if (ImGui::IsItemClicked())
	{
		ec->playModeState = EngineContext::EPlayModeState::PlayMode;
	}
	offset += dtOffset;
	ImGui::SameLine(offset);
	ImGui::SetNextItemAllowOverlap();
	activeButtonBG.w = ec->playModeState == EngineContext::EPlayModeState::PauseMode;
	ImGui::ImageButton((void*)pauseBtnSRV.Get(), { 20,20 }, ImVec2(0, 0), ImVec2(1, 1), -1, activeButtonBG);
	if (ImGui::IsItemClicked() &&
		ec->playModeState == EngineContext::EPlayModeState::PlayMode)
	{
		ec->playModeState = EngineContext::EPlayModeState::PauseMode;
	}
	offset += dtOffset;
	ImGui::SameLine(offset);
	ImGui::SetNextItemAllowOverlap();

	ImGui::ImageButton((void*)stopBtnSRV.Get(), { 20,20 });

	if (ImGui::IsItemClicked() &&
		ec->playModeState != EngineContext::EPlayModeState::EditorMode)
	{
		ec->playModeState = EngineContext::EPlayModeState::EditorMode;
	}
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

