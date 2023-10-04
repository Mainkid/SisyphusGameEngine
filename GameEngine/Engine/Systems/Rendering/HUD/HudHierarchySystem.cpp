#include "HudHierarchySystem.h"
#include "../../Core/ServiceLocator.h"
#include "../../EngineContext.h"
#include "../../../Components/GameObjectComp.h"
#include "../../../Scene/GameObjectHelper.h"
#include "../../Components/TransformComponent.h"

void HudHierarchySystem::Init()
{
    this->ec = ServiceLocator::instance()->Get<EngineContext>();
    this->windowID = "Hierarchy";
}

void HudHierarchySystem::Run()
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
    ImGui::InvisibleButton("dummyBtn", ImVec2(windowSize.x-cursorPos.x-paddingVal, windowSize.y-cursorPos.y- paddingVal));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
        {
            const entt::entity* item = (const entt::entity*)payload->Data;
            GameObjectHelper::SetParent(_ecs, *item, entt::null);
            ImGui::EndDragDropTarget();
        }
    }
    ImGui::End();
}

void HudHierarchySystem::Destroy()
{
    
}

void HudHierarchySystem::RenderTree(std::set<entt::entity> entities)
{
    for (auto ent : entities)
    {
        auto [go, transform] = _ecs->get<GameObjectComp, TransformComponent>(ent);

        ImGuiTreeNodeFlags treeFlags = ((ent == ec->selectedEntityID) ? ImGuiTreeNodeFlags_Selected : 0);
        treeFlags |= (transform.children.size() == 0 ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None);
        treeFlags |= baseFlags;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)ent, treeFlags,
            go.Name.c_str());
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            ec->selectedEntityID = ent;

        if (ImGui::BeginDragDropSource())
        {
            const entt::entity item = entt::entity(ent);
            ImGui::SetDragDropPayload("_TREENODE", &item, sizeof(entt::entity));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
            {
                const entt::entity* item = (const entt::entity*)payload->Data;
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


