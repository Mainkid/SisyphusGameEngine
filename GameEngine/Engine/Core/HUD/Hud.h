#pragma once

#include "../../../vendor/Delegates.h"
#include "Widgets/ViewportWidget.h"
#include "Widgets/HierarchyWidget.h"
#include "Widgets/ContentBrowser.h"
#include "Widgets/PropertiesWidget.h"


#include "Widget.h"
#include <memory>
#include <set>

struct HardwareContext;
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
	void CleanupRenderTarget();
	void CreateRenderTarget();


	MulticastDelegate<float> ViewportResizedEvent;
	MulticastDelegate<entt::entity> UpdateSelectedEntityEvent;
private:
	int windowHeight = 0;
	int windowWidth = 0;
	std::set<std::unique_ptr<Widget>> widgets;
	std::string focusedWindowName="NONE";
	entt::entity selectedEntityID = entt::null;
	HardwareContext* hc;
};

