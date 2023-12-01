#include "HudHierarchySystem.h"
#include "../../Core/ServiceLocator.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Components/GameObjectComp.h"
#include "../../../Events/SyEditorCameraMoveToTarget.h"
#include "../../../Scene/GameObjectHelper.h"
#include "../../Components/TransformComponent.h"

SyResult HudHierarchySystem::Init()
{
	this->ec = ServiceLocator::instance()->Get<EngineContext>();
	this->windowID = "Hierarchy";
	return SyResult();
}

SyResult HudHierarchySystem::Run()
{
	ImGui::Begin(windowID.c_str());

	std::set<entt::entity> rootEntities;
	auto view = _ecs->view<GameObjectComp, TransformComponent>();
	for (auto ent : view)
	{
		auto transform = view.get<TransformComponent>(ent);
		if (transform.parent == entt::null)
			rootEntities.insert(ent);
	}
	RenderTree(rootEntities);

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 cursorPos = ImGui::GetCursorPos();
	int paddingVal = 10;
	ImGui::InvisibleButton("dummyBtn", ImVec2(windowSize.x - cursorPos.x - paddingVal,
	                                          windowSize.y - cursorPos.y - paddingVal));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
		{
			auto item = (const entt::entity*)payload->Data;
			GameObjectHelper::SetParent(_ecs, *item, entt::null);
			ImGui::EndDragDropTarget();
		}
	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_C))
	{
		ec->hudData.copyBufferEntityIDs = ec->hudData.selectedEntityIDs;
	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_V))
	{
		GameObjectHelper::CopyEntity(_ecs, ec->hudData.copyBufferEntityIDs);
	}

	ImGui::End();
	return SyResult();
}

SyResult HudHierarchySystem::Destroy()
{
	return SyResult();
}

void HudHierarchySystem::RenderTree(std::set<entt::entity> entities)
{
	for (auto ent : entities)
	{
		auto [go,transform] = _ecs->get<GameObjectComp, TransformComponent>(ent);

		ImGuiTreeNodeFlags treeFlags = (
			(ec->hudData.selectedEntityIDs.count(ent) > 0) ? ImGuiTreeNodeFlags_Selected : 0);
		treeFlags |= (_ecs->get<TransformComponent>(ent).children.size() == 0)
			             ? ImGuiTreeNodeFlags_Leaf
			             : ImGuiTreeNodeFlags_None;
		bool opened;
		if (isRenaiming == ent)
		{
			//ImGui::SetNextItemAllowOverlap();
			//ImGui::SameLine();
			if (ImGui::InputText("Renaming", renamingString, IM_ARRAYSIZE(renamingString), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (renamingString[0]!='\0')
					_ecs->get<GameObjectComp>(ent).Name = renamingString;
				
				isRenaiming = entt::null;
			}
			opened = false;
		}
		else
			 opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)ent, treeFlags,
		                                go.Name.c_str());

		

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("HierarchyPopUp");
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			CallEvent<SyEditorCameraMoveToTarget>("SyEditorCameraMoveToTarget", ent);
		}

		ProcessPopUp();
		
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			if (!ImGui::IsKeyDown(ImGuiMod_Ctrl))
				ec->hudData.selectedEntityIDs.clear();
			if (ec->hudData.selectedEntityIDs.count(ent) > 0)
				ec->hudData.selectedEntityIDs.erase(ent);
			else
				ec->hudData.selectedEntityIDs.insert(ent);
			ec->hudData.selectedContent.assetType = EAssetType::ASSET_NONE;
		}

		if (ImGui::BeginDragDropSource())
		{
			const auto item = entt::entity(ent);
			ImGui::SetDragDropPayload("_TREENODE", &item, sizeof(entt::entity));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
			{
				auto item = (const entt::entity*)payload->Data;
				GameObjectHelper::SetParent(_ecs, *item, ent);
				ImGui::EndDragDropTarget();
			}
		}

		if (opened)
		{
			RenderTree(transform.children);
			
			ImGui::TreePop();
		}


	}
}

void HudHierarchySystem::ProcessPopUp()
{
	if (ImGui::BeginPopup("HierarchyPopUp"))
	{
		if (ImGui::MenuItem("Cut"))
		{
			int a = 0;
		}
		if (ImGui::MenuItem("Copy"))
		{
			ec->hudData.copyBufferEntityIDs = ec->hudData.selectedEntityIDs;
		}
		if (ImGui::MenuItem("Paste"))
		{
			GameObjectHelper::CopyEntity(_ecs, ec->hudData.copyBufferEntityIDs, entt::null);
		}
		if (ImGui::MenuItem("Paste As Child"))
		{
			GameObjectHelper::CopyEntity(_ecs, ec->hudData.copyBufferEntityIDs, *ec->hudData.selectedEntityIDs.begin());
		}
		if (ImGui::MenuItem("Rename"))
		{
			if (ec->hudData.selectedEntityIDs.size() > 0)
			{
				isRenaiming = *ec->hudData.selectedEntityIDs.begin();
				strcpy(renamingString, _ecs->get<GameObjectComp>(*ec->hudData.selectedEntityIDs.begin()).Name.c_str());
			}
		}
		if (ImGui::MenuItem("Delete"))
		{
			for (auto gameobject : ec->hudData.selectedEntityIDs)
			{
				GameObjectHelper::Destroy(_ecs,gameobject);
			}

			ec->hudData.selectedEntityIDs.clear();
		}
		if (ImGui::MenuItem("Create Empty"))
		{
			for (auto gameobject : ec->hudData.selectedEntityIDs)
			{
				auto entt = GameObjectHelper::Create(_ecs, "GameObject", Vector3::Zero);
				GameObjectHelper::SetParent(_ecs, entt, gameobject);
			}
		}
		if (ImGui::MenuItem("Create Empty Parent"))
		{
			for (auto gameobject : ec->hudData.selectedEntityIDs)
			{

				auto entt = GameObjectHelper::Create(_ecs, "GameObject", Vector3::Zero);
				
				GameObjectHelper::SetParent(_ecs, entt, (entt::entity)_ecs->get<TransformComponent>(gameobject).parent);
				GameObjectHelper::SetParent(_ecs, gameobject, entt);
			}
		}

		ImGui::EndPopup();
	}

	/* switch (selectedCategory)
	 {
	 case CreatePopup:
	     switch (selected_Popup)
	     {
	     case 0:
	         ResourceHelper::ConstructFile(curDirectory.string() + "\\NewFolder");
	         break;
	     case 1:
	         auto createdFile = ResourceHelper::ConstructFile(curDirectory.string() + "\\Material.mat");
	         ResourceHelper::CreateMetaFile_(createdFile, EAssetType::ASSET_MATERIAL);
	         ResourceHelper::FillFileWithBaseData(createdFile, EAssetType::ASSET_MATERIAL);
	         rs->resourceLibrary[rs->GetUUIDFromPath(createdFile)] = { EAssetType::ASSET_MATERIAL,createdFile.string() };
	         break;
	     }
	     InitializePathFileViews(curDirectory);
	     break;
	 case MainPopup:
	     switch (selected_Popup)
	     {
	     case 0:
	         for (auto& selectedFile : selectedFiles)
	         {
 
	             CheckRemovingResourceReferences(selectedFile);
	             ResourceHelper::RemoveFile(selectedFile);
	         }
	         rs->updateContentBrowser.Broadcast(true);
	         break;
	     case 1:
	         renamingFileName = *selectedFiles.begin();
	         break;
	     case 2:
	         std::string resPath = curDirectory.string();
	         std::replace(resPath.begin(), resPath.end(), '/', '\\');
	         std::string command = "explorer /select," + resPath;
	         system(command.c_str());
	     }
	     break;*/
	//}
}
