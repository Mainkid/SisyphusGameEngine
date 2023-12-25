#include "HudViewportSystem.h"
#include "../../../Core/ServiceLocator.h"
#include "../../Resources/ResourceService.h"
#include "../RenderHelper.h"
#include "../../Core/Rendering/RenderTarget.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../../Core/ResourcePath.h"
#include "../../../Scene/CameraHelper.h"
#include "../../../Scene/GameObjectHelper.h"
#include "../../Mesh/Components/MeshComponent.h"
#include "../../Events/SyHotReloadEvent.h"
#include "../../Events/SyPlayModeEndedEvent.h"
#include "../../Events/SyPlayModeStartedEvent.h"
#include "../../Events/SySceneLoadEvent.h"
#include "../../../Components/ImageBasedLightingComponent.h"
#include "../../../Scene/Prefab.h"
#include "../../../Scene/Scene.h"
#include "json.hpp"

SyResult HudViewportSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rs = ServiceLocator::instance()->Get<ResourceService>();
	windowID = "Viewport";
	InitSRV();
	return SyResult();
	
}

SyResult HudViewportSystem::Run()
{
	ImGui::Begin(windowID.c_str());
	DrawMainMenuBar();

	ImGui::BeginChild("ViewportTools",ImVec2(ImGui::GetContentRegionAvail().x,26));
	ImVec2 startCursorPos = ImGui::GetCursorScreenPos();
	DrawPlayMode(startCursorPos);
	DrawViewportTools(startCursorPos);
	ImGui::EndChild();
	ImGui::BeginChild("ViewportChild");
	ImVec2 imgSize = ImGui::GetContentRegionAvail();
	startCursorPos = ImGui::GetCursorScreenPos();
	

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
	
	//std::cout << (int)hoverState << std::endl;

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left)&& !ImGuizmo::IsOver()&& hoverState==EHoveringState::Viewport)
	{
		
		int pixelColorR =RenderHelper::GetPixelValue(x, y).x;
		if (pixelColorR != -1)
		{
			if (!ImGui::IsKeyDown(ImGuiMod_Ctrl))
				ec->hudData.selectedEntityIDs.clear();

			if (ec->hudData.selectedEntityIDs.count(entt::entity(pixelColorR)) > 0)
				ec->hudData.selectedEntityIDs.erase(entt::entity(pixelColorR));
			else
				ec->hudData.selectedEntityIDs.insert(entt::entity(pixelColorR));
			ec->hudData.selectedContent.assetType = EAssetType::ASSET_NONE;
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
				auto [camera, cameraTransform] = CameraHelper::Find(_ecs, ec->playModeState);
				Vector3 pos = cameraTransform.localPosition + camera.forward * 5.0f;
				auto go = GameObjectHelper::CreateMesh(_ecs, uuid,pos);
			}
			else if (rs->resourceLibrary[uuid].assetType == EAssetType::ASSET_MATERIAL)
			{
				auto enttID = RenderHelper::GetPixelValue(x, y);
				if (enttID.z == int(EAssetType::ASSET_MESH))
				{
					MeshComponent& meshComp = _ecs->get<MeshComponent>(static_cast<entt::entity>(enttID.x));
					meshComp.materialUUIDs[enttID.y] = uuid;
				}
			}
			else if (rs->resourceLibrary[uuid].assetType == EAssetType::ASSET_PREFAB)
			{
				auto prefab = std::static_pointer_cast<Prefab>(rs->LoadResource(uuid));
				std::cout << std::endl;
				//TODO: prefab drag drop
			}
			else if (rs->resourceLibrary[uuid].assetType == EAssetType::ASSET_SCENE)
			{
				auto scene = std::static_pointer_cast<Scene>(rs->LoadResource(uuid));
				//TODO: scene drag drop
			}
			else if (rs->resourceLibrary[uuid].assetType == EAssetType::ASSET_CUBEMAP)
			{

				GameObjectHelper::CreateSkybox(_ecs, uuid);

			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	auto [camera, cameraTf] = CameraHelper::Find(_ecs,ec->playModeState);
	camera.mouseWheel = io.MouseWheel;

	//Gizmos
	if (ec->hudData.selectedEntityIDs.size()!=0)
	{
		Matrix deltaMatrix;
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		
		auto viewMat = camera.view;
		auto projMat = camera.projection;
		auto& selectedTransform = _ecs->get<TransformComponent>(*ec->hudData.selectedEntityIDs.begin());
		auto transformMat = selectedTransform.transformMatrix;

		ImGuizmo::MODE mode = ImGuizmo::LOCAL;
		if (isWorldSpace)
			mode = ImGuizmo::WORLD;

		for (int i = 0; i < 3; i++)
			snap[i] = snap_values[gridSnapParam];

		auto res = ImGuizmo::Manipulate(&viewMat._11, &projMat._11, guizmoType, mode, &transformMat._11, &deltaMatrix._11, mode == ImGuizmo::WORLD ? snap : NULL);
		if (ImGuizmo::IsUsing())
		{
			for (auto& id: ec->hudData.selectedEntityIDs)
			{
				Matrix transformMat;
				TransformComponent& tc = _ecs->get<TransformComponent>(id);
				Vector3 translation, scale, translationL, scaleL;
				Quaternion rotation, rotationL;
				transformMat = tc.transformMatrix * deltaMatrix;
				transformMat = transformMat * TransformHelper::ConstructInverseParentTransform(tc);
				transformMat.Decompose(scale, rotation, translation);
				Vector3 deltaRotation = rotation.ToEuler() - tc.localRotation;
				tc.localRotation = tc.localRotation + deltaRotation;
				tc.localPosition = translation;
				tc.localScale = scale;
			}
		}



	}
	
	if (ImGui::IsKeyDown(ImGuiKey_R))
		guizmoType = ImGuizmo::OPERATION::ROTATE;
	if (ImGui::IsKeyDown(ImGuiKey_T))
		guizmoType = ImGuizmo::OPERATION::TRANSLATE;
	if (ImGui::IsKeyDown(ImGuiKey_E))
		guizmoType = ImGuizmo::OPERATION::SCALE;

	
	
	hoverState = EHoveringState::Viewport;
	
	ProcessInput();
	

	HandleResize();
	ImGui::EndChild();
	ImGui::End();
	return SyResult();
}

SyResult HudViewportSystem::Destroy()
{
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

	ImageLoader::LoadTextureFromFile(worldIconPath.string().c_str(), worldSRV.GetAddressOf(), &width, &height);
	viewportIconSRV[EViewportTool::WorldSpace] = worldSRV.Get();

	ImageLoader::LoadTextureFromFile(localIconPath.string().c_str(), localSRV.GetAddressOf(), &width, &height);
	viewportIconSRV[EViewportTool::LocalSpace] = localSRV.Get();

	ImageLoader::LoadTextureFromFile(gridIconPath.string().c_str(), gridSRV.GetAddressOf(), &width, &height);
	viewportIconSRV[EViewportTool::Grid] = gridSRV.Get();

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
	static EngineContext::EPlayModeState prevPlayMode = EngineContext::EPlayModeState::PauseMode;


	auto newWidgetSize = ImGui::GetWindowSize();
	if (widgetSize.x != newWidgetSize.x || widgetSize.y != newWidgetSize.y || ec->playModeState!=prevPlayMode)
	{
		widgetSize = newWidgetSize;

		auto [cc,_] = CameraHelper::Find(_ecs,ec->playModeState);
		cc.aspectRatio = widgetSize.x / widgetSize.y;
	}
	prevPlayMode = ec->playModeState;
}

void HudViewportSystem::DrawMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open scene"))
			{
				OPENFILENAME ofn;
				WCHAR* szFile = new WCHAR[512];
				WCHAR* szFileTitle = new WCHAR[512];
				memset(&ofn, 0, sizeof(ofn));
				memset(szFile, 0, sizeof(WCHAR) * 512);
				memset(szFileTitle, 0, sizeof(WCHAR) * 512);

				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hc->window->GetHWND();
				ofn.lpstrFilter = L"All File\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(WCHAR) * 512;
				ofn.lpstrFileTitle = szFileTitle;
				ofn.nMaxFileTitle = sizeof(WCHAR) * 512;
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;

				// press the OK button
				BOOL ok = GetOpenFileName(&ofn);
				if (ok) {
				
					rs->LoadSceneFromFile(ofn.lpstrFile, _ecs);
					CallEvent<SySceneLoadEvent>("SySceneLoadEvent");
					MessageBox(hc->window->GetHWND(), L"Plumb", L"Plumb", MB_OK);
				}

				delete[]szFile;
				delete[]szFileTitle;
			
			}
			if (ImGui::MenuItem("Save scene"))
			{
				OPENFILENAME ofn;
				WCHAR* szFile = new WCHAR[512];
				WCHAR* szFileTitle = new WCHAR[512];
				memset(&ofn, 0, sizeof(ofn));
				memset(szFile, 0, sizeof(WCHAR) * 512);
				memset(szFileTitle, 0, sizeof(WCHAR) * 512);

				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hc->window->GetHWND();
				ofn.lpstrFilter = L"All File\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(WCHAR) * 512;
				ofn.lpstrFileTitle = szFileTitle;
				ofn.nMaxFileTitle = sizeof(WCHAR) * 512;
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;

				// press the OK button
				BOOL ok = GetSaveFileName(&ofn);
				if (ok) {
					rs->SaveSceneToFile(ofn.lpstrFile, _ecs);
					MessageBox(hc->window->GetHWND(), L"Plumb", L"Plumb", MB_OK);
				}

				delete[]szFile;
				delete[]szFileTitle;

			}

			if (ImGui::MenuItem("Hot Reload"))
			{
				CallEvent<SyHotReloadEvent>("SyHotreloadEvent");
			}
			
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			auto [camera, cameraTransform] = CameraHelper::Find(_ecs,ec->playModeState);

			Vector3 pos = Vector3(cameraTransform._position.x, cameraTransform._position.y, cameraTransform._position.z) +
				Vector3(camera.forward.x, camera.forward.y, camera.forward.z) * 3;

			if (ImGui::BeginMenu("3D object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					GameObjectHelper::CreateMesh(_ecs, rs->GetUUIDFromPath(cubeMeshPath),pos);
				}
				else if (ImGui::MenuItem("Sphere"))
				{
					GameObjectHelper::CreateMesh(_ecs, rs->GetUUIDFromPath(sphereMeshPath),pos);
				}
				else if (ImGui::MenuItem("Plane"))
				{
					GameObjectHelper::CreateMesh(_ecs, rs->GetUUIDFromPath(planeMeshPath),pos);
				}
				else if (ImGui::MenuItem("Cylinder"))
				{
					GameObjectHelper::CreateMesh(_ecs, rs->GetUUIDFromPath(cylinderMeshPath),pos);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Lights"))
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					GameObjectHelper::CreateLight(_ecs,ELightType::Directional,pos);
				}

				if (ImGui::MenuItem("Point Light"))
				{
					GameObjectHelper::CreateLight(_ecs,ELightType::PointLight,pos);
				}

				if (ImGui::MenuItem("Ambient Light"))
				{
					GameObjectHelper::CreateLight(_ecs,ELightType::Ambient,pos);
				}

				ImGui::EndMenu();
			
			}
			if (ImGui::BeginMenu("Effects"))
			{
				if (ImGui::MenuItem("Particle System"))
				{
					GameObjectHelper::CreateParticleSystem(_ecs);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Audio"))
			{
				if (ImGui::MenuItem("Sound Component"))
				{
					GameObjectHelper::CreateSoundComponent(_ecs);
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Open", "Ctrl+O")) {}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Graphics"))
		{
			if (ImGui::BeginMenu("Add..."))
			{
				
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Bake Lighting"))
			{
				
			}

			ImGui::EndMenu();
		}


		ImGui::EndMainMenuBar();
	}
}

void HudViewportSystem::ProcessInput()
{
	if (ImGui::IsKeyDown(ImGuiKey_Delete))
	{
		if (ec->hudData.selectedEntityIDs.size() != 0)
		{
			for (auto& go : ec->hudData.selectedEntityIDs)
			{
				GameObjectHelper::Destroy(_ecs,go);
			}
			
			ec->hudData.selectedEntityIDs.clear();
		}
	}
	
	if (ImGui::Shortcut(ImGuiMod_Ctrl| ImGuiKey_C))
	{
		ec->hudData.copyBufferEntityIDs = ec->hudData.selectedEntityIDs;
	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_V))
	{

		
		GameObjectHelper::CopyEntity(_ecs, ec->hudData.copyBufferEntityIDs);
		
	}
	

}

void HudViewportSystem::DrawPlayMode(ImVec2 cursorStartPostion)
{
	if (ImGui::IsItemHovered())
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
		CallEvent<SyPlayModeStartedEvent>("SyPlayModeStartedEvent");
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
		CallEvent<SyPauseModeEvent>("SyPauseModeEvent");
	}
	offset += dtOffset;
	ImGui::SameLine(offset);
	ImGui::SetNextItemAllowOverlap();

	ImGui::ImageButton((void*)stopBtnSRV.Get(), { 20,20 });

	if (ImGui::IsItemClicked() &&
		ec->playModeState != EngineContext::EPlayModeState::EditorMode)
	{
		ec->playModeState = EngineContext::EPlayModeState::EditorMode;
		CallEvent<SyPlayModeEndedEvent>("SyPlayModeEndedEvent");
	}
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void HudViewportSystem::DrawViewportTools(ImVec2 startCursorPos)
{
	

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


	ImGui::SameLine(0,12);
	ImGui::SetNextItemAllowOverlap();
	ID3D11ShaderResourceView* srv;
	if (isWorldSpace)
		srv = viewportIconSRV[EViewportTool::WorldSpace];
	else
		srv = viewportIconSRV[EViewportTool::LocalSpace];
	
	ImGui::ImageButton((void*)srv, {20,20});
	if (ImGui::IsItemHovered())
	{
		hoverState = EHoveringState::None;

	}
	if (ImGui::IsItemClicked())
	{
		isWorldSpace = !isWorldSpace;
	}


	ImGui::SameLine();
	ImGui::SetNextItemAllowOverlap();
	ImGui::ImageButton((void*)viewportIconSRV[EViewportTool::Grid], { 20,20 });
	if (ImGui::IsItemHovered())
	{
		hoverState = EHoveringState::None;

	}
	if (ImGui::IsItemClicked())
	{
		ImGui::OpenPopup("Grid settings");
	}

	if (ImGui::BeginPopupContextItem("Grid settings"))
	{
		static float value = 0;
		ImGui::Text("Grid snapping");
		const char* elem_name = (gridSnapParam >= 0 && gridSnapParam < 6) ? elems_names[gridSnapParam] : "Unknown";
		ImGui::SliderInt("step", &gridSnapParam, 0, 6 - 1, elem_name); // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

}

