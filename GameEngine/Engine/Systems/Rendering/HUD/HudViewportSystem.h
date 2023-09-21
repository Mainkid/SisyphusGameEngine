#pragma once
#include "../../ISystem.h"
#include <wrl.h>
#include "../../../../vendor/entt/entt.hpp"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../../vendor/ImGui/imgui_impl_win32.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../../../Tools/ImageLoader.h"
#include "../../../Systems/TransformHelper.h"
#include <string>
#include <map>
#include <filesystem>

struct EngineContext;
struct HardwareContext;
struct RenderContext;


class HudViewportSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;

	void InitSRV();
	void HandleResize();
	void drawPlayMode(ImVec2 cursorStartPos);

	std::string windowID;
	bool isFocused;
	enum class EHoveringState
	{
		None,
		TransformGizmo,
		RotateGizmo,
		ScaleGizmo,
		Viewport,
		PlayMode
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

	const std::filesystem::path playButtonPath = "Engine/Assets/HUD/Viewport/PlayButton.png";
	const std::filesystem::path stopButtonPath = "Engine/Assets/HUD/Viewport/StopButton.png";
	const std::filesystem::path pauseButtonPath = "Engine/Assets/HUD/Viewport/PauseButton.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> playBtnSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stopBtnSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pauseBtnSRV;


};

