#include "HierarchyWidget.h"

HierarchyWidget::HierarchyWidget(Hud* _hud)
{

	this->hud = _hud;
	this->windowID = "Hierarchy";
	hud->UpdateSelectedEntityEvent.AddRaw(this, &HierarchyWidget::UpdateSelectedEntity);
}

void HierarchyWidget::Render()
{
	ImGui::Begin(windowID.c_str());
	Widget::Render();
	

	std::set<entt::entity> tmpSet;
	for (auto& gameObject : EngineCore::instance()->scene->gameObjects)
	{
		if (!std::get<0>(gameObject.second->GetParentObjects()))
		{
			tmpSet.insert(gameObject.first);
			
		}
	}
	
	RenderTree(tmpSet);

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 cursorPos = ImGui::GetCursorPos();
	int paddingVal = 10;
	ImGui::InvisibleButton("dummyBtn", ImVec2(windowSize.x-cursorPos.x-paddingVal, windowSize.y-cursorPos.y- paddingVal));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
		{
			
			const entt::entity* item = (const entt::entity*)payload->Data;
			EngineCore::instance()->scene->SetParent(*item, entt::null);
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();
}

void HierarchyWidget::UpdateSelectedEntity(entt::entity)
{
}

void HierarchyWidget::RenderTree(std::set<entt::entity>& gameObjectsVector)
{

	for (auto& gameObjectID : gameObjectsVector)
	{
		ImGuiTreeNodeFlags treeFlags = ((gameObjectID == hud->selectedEntityID) ? ImGuiTreeNodeFlags_Selected : 0);
		treeFlags |= (EngineCore::instance()->scene->gameObjects[gameObjectID]->GetChildrenObjects().size() == 0) ?
			ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None;
		treeFlags |= baseFlags;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)gameObjectID, treeFlags,
			EngineCore::instance()->scene->gameObjects[gameObjectID]->name.c_str());
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			//selectedEntity = gameObjectID;
			hud->UpdateSelectedEntityEvent.Broadcast(gameObjectID);

		if (ImGui::BeginDragDropSource())
		{
			const entt::entity item = entt::entity(gameObjectID);
			ImGui::SetDragDropPayload("_TREENODE", &item, sizeof(entt::entity));
			
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
			{
				const entt::entity* item = (const entt::entity*)payload->Data;
				EngineCore::instance()->scene->SetParent(*item, gameObjectID);
				ImGui::EndDragDropTarget();
			}
		}

		if (opened)
		{
			RenderTree(EngineCore::instance()->scene->gameObjects[gameObjectID]->GetChildrenObjects());
			ImGui::TreePop();
		}
	}
}
