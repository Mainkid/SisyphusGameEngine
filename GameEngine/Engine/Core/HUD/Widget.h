#pragma once
#include "../../DirectXSDK/ImGui/imgui.h"
#include "../../DirectXSDK/ImGui/imgui_impl_dx11.h"
#include "../../DirectXSDK/ImGui/imgui_impl_win32.h"

class Widget
{
public:
	Widget() = default;
	virtual void Render() = 0;
	virtual void Initialize() {};
	virtual ~Widget() = default;
};

