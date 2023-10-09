#include "HudHierarchySystem.h"
#include "../../Core/ServiceLocator.h"
#include "../../EngineContext.h"

SyResult HudHierarchySystem::Init()
{
    this->ec = ServiceLocator::instance()->Get<EngineContext>();
    this->windowID = "Hierarchy";
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "HudHierarchy system initialization successful.");
    return SyResult();
}

SyResult HudHierarchySystem::Run()
{
    ImGui::Begin(windowID.c_str());
	
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
    return SyResult();
}

SyResult HudHierarchySystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "HudHierarchy system destruction successful.");
    return SyResult();
}

void HudHierarchySystem::RenderTree(std::set<entt::entity>& gameObjectsVector)
{
    for (auto& gameObjectID : gameObjectsVector)
    {
        ImGuiTreeNodeFlags treeFlags = ((gameObjectID == ec->selectedEntityID) ? ImGuiTreeNodeFlags_Selected : 0);
        treeFlags |= (ec->scene->registry.get<TransformComponent>(gameObjectID).children.size() == 0) ?
            ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None;
        treeFlags |= baseFlags;

        DataComponent& dc = ec->scene->registry.get<DataComponent>(gameObjectID);
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)gameObjectID, treeFlags,
            dc.name.c_str());
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            ec->selectedEntityID=gameObjectID;

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


