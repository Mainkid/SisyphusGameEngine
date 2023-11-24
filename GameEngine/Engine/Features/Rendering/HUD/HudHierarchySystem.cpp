#include "HudHierarchySystem.h"
#include "../../Core/ServiceLocator.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Components/GameObjectComp.h"
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

        ImGuiTreeNodeFlags treeFlags = ((ec->hudData.selectedEntityIDs.count(ent) > 0) ? ImGuiTreeNodeFlags_Selected : 0);
        treeFlags |= (_ecs->get<TransformComponent>(ent).children.size() == 0) ?
            ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)ent, treeFlags,
            go.Name.c_str());


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


