#pragma once
#include "../../DirectXSDK/ImGui/imgui.h"
#include "../../DirectXSDK/ImGui/imgui_impl_dx11.h"
#include "../../DirectXSDK/ImGui/imgui_impl_win32.h"

#include "Widgets/ViewportWidget.h"

#include "Widget.h"
#include <memory>
#include <set>

class EngineCore;

class Hud
{
public:
	Hud() = default;
	void Initialize();
	void Render();

private:
	std::set<std::unique_ptr<Widget>> widgets;
};

