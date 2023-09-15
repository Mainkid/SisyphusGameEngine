#pragma once

#include "../Widget.h"
#include "../Hud.h"
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include "../../../../vendor/entt/entt.hpp"
//#include "../../../DirectXSDK/entt/entt.hpp"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../../vendor/ImGui/imgui_impl_win32.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../../../Tools/ImageLoader.h"
#include "../../../Systems/TransformHelper.h"
#include <string>
#include <map>
#include <filesystem>

struct HardwareContext;

class ViewportWidget: public Widget
{
public:
	ViewportWidget(Hud* _hud);
	void InitSRV();
	void Render() override;
	void HandleResize();
	void GetInput() override;
	void UpdateSelectedEntity(entt::entity);

private:
	HardwareContext* hc;
	enum class EHoveringState
	{
		None,
		TransformGizmo,
		RotateGizmo,
		ScaleGizmo,
		Viewport
	};

	ImGuizmo::OPERATION guizmoType= ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE guizmoMode = ImGuizmo::MODE::WORLD;
	ImVec2 widgetSize;
	int x;
	int y;

	EHoveringState hoverState = EHoveringState::None;
	std::map<ImGuizmo::OPERATION, ID3D11ShaderResourceView*> guizmoIconSRV;

	const std::filesystem::path translateIconPath="Engine/Assets/HUD/Viewport/translateIcon.png";
	const std::filesystem::path rotateIconPath= "Engine/Assets/HUD/Viewport/rotateIcon.png";
	const std::filesystem::path scaleIconPath = "Engine/Assets/HUD/Viewport/scaleIcon.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> translateSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rotateSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scaleSRV;
};

