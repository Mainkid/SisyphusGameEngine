#pragma once
#include "../../../vendor/ImGui/imgui.h"
#include "../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../vendor/ImGui/imgui_impl_win32.h"

#include <string>

class Hud;

class Widget
{
public:
	
	Widget() = default;
	virtual void Render();
	virtual void Initialize() {};
	virtual void GetInput() {};
	bool IsWindowFocused() { return isFocused; }
	virtual ~Widget() = default;

	
protected:
	std::string windowID;
	bool isFocused;
	Hud* hud;
};

