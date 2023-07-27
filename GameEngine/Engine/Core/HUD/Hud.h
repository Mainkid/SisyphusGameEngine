#pragma once
#include "../../DirectXSDK/ImGui/imgui.h"
#include "../../DirectXSDK/ImGui/imgui_impl_dx11.h"
#include "../../DirectXSDK/ImGui/imgui_impl_win32.h"
#include "../../DirectXSDK/ImGuizmo/ImGuizmo.h"
#include "../../DirectXSDK/Delegates.h"
#include "Widgets/ViewportWidget.h"
#include "Widgets/HierarchyWidget.h"
#include "Widgets/ContentBrowser.h"
#include "Widgets/PropertiesWidget.h"


#include "Widget.h"
#include <memory>
#include <set>

class EngineCore;
class ViewportWidget;

class Hud
{
	friend class ViewportWidget;
	friend class HierarchyWidget;

public:
	Hud() = default;
	void Initialize();
	void Render();
	void Shutdown();

	std::string GetFocusedWindowName();
	void SetFocusedWindowName(std::string _str); //TODO: Инкапсулировать!
	void UpdateSelectedEntity(entt::entity);

	MulticastDelegate<float> ViewportResizedEvent;
	MulticastDelegate<entt::entity> UpdateSelectedEntityEvent;
private:
	std::set<std::unique_ptr<Widget>> widgets;
	std::string focusedWindowName="NONE";
	entt::entity selectedEntityID = entt::null;
};

