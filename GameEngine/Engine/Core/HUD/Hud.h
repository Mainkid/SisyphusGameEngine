#pragma once

#include "../../../vendor/Delegates.h"
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
	friend class PropertiesWidget;

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

