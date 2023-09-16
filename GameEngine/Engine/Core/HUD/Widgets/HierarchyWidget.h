#pragma once
#include "../Widget.h"
#include "../../EngineCore.h"

class GameObject;
struct EngineContext;

class HierarchyWidget: public Widget
{
public:
	HierarchyWidget(Hud* _hud);
	void Render() override;
	void GetInput() override {};
	void UpdateSelectedEntity(entt::entity);

private:
	EngineContext* ec;
	const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	void RenderTree(std::set<entt::entity>& gameObjectsVector);
	
};

