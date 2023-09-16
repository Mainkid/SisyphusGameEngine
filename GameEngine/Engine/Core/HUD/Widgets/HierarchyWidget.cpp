#include "HierarchyWidget.h"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../../vendor/ImGui/imgui_impl_win32.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../../../Core/ServiceLocator.h"'
#include "../../../Systems/EngineContext.h"

HierarchyWidget::HierarchyWidget(Hud* _hud)
{
	this->ec = ServiceLocator::instance()->Get<EngineContext>();
	this->hud = _hud;
	this->windowID = "Hierarchy";
	hud->UpdateSelectedEntityEvent.AddRaw(this, &HierarchyWidget::UpdateSelectedEntity);
}

void HierarchyWidget::Render()
{
	ImGui::Begin(windowID.c_str());
	Widget::Render();
	
	std::set<entt::entity> tmpSet;
	for (auto& id : ec->scene->gameObjects)
	{
		if (ec->scene->registry.get<TransformComponent>(id).parent==entt::null) //???
		{
			tmpSet.insert(id);
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
			ec->scene->SetParent(*item, entt::null);
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
		treeFlags |= (ec->scene->registry.get<TransformComponent>(gameObjectID).children.size() == 0) ?
			ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None;
		treeFlags |= baseFlags;

		DataComponent& dc = ec->scene->registry.get<DataComponent>(gameObjectID);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)gameObjectID, treeFlags,
			dc.name.c_str());
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
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
				ec->scene->SetParent(*item, gameObjectID);
				ImGui::EndDragDropTarget();
			}
		}

		if (opened)
		{
			RenderTree(ec->scene->registry.get<TransformComponent>(gameObjectID).children);
			ImGui::TreePop();
		}
	}
}
