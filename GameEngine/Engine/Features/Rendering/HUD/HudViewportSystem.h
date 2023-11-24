#pragma once
#include "../../Core/ECS/SystemBase.h"
#include <wrl.h>
#include "../../../../vendor/entt/entt.hpp"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../../vendor/ImGui/imgui_internal.h"
#include "../../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../../vendor/ImGui/imgui_impl_win32.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../../../Core/Tools/ImageLoader.h"
#include "../../../Systems/TransformHelper.h"
#include <string>
#include <map>
#include <filesystem>
#include <boost/uuid/uuid.hpp>
#include <boost/lexical_cast.hpp>
#include <commdlg.h>

struct EngineContext;
struct HardwareContext;
struct RenderContext;
class ResourceService;


class HudViewportSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;
	ResourceService* rs;

	void InitSRV();
	void HandleResize();
	void DrawPlayMode(ImVec2 cursorStartPos);
	void DrawViewportTools(ImVec2 startCursorPos);
	void DrawMainMenuBar();
	void ProcessInput();

	std::string windowID;
	bool isFocused;
	bool isWorldSpace = true;

	enum class EHoveringState
	{
		None,
		TransformGizmo,
		RotateGizmo,
		ScaleGizmo,
		Viewport,
		PlayMode
	};

	enum class EViewportTool
	{
		None,
		WorldSpace,
		LocalSpace,
		Grid
	};

	ImGuizmo::OPERATION guizmoType= ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE guizmoMode = ImGuizmo::MODE::WORLD;
	ImVec2 widgetSize;
	int x;
	int y;

	EHoveringState hoverState = EHoveringState::None;
	std::map<ImGuizmo::OPERATION, ID3D11ShaderResourceView*> guizmoIconSRV;
	std::map<EViewportTool, ID3D11ShaderResourceView*> viewportIconSRV;

	const std::filesystem::path translateIconPath="Engine/Assets/HUD/Viewport/translateIcon.png";
	const std::filesystem::path rotateIconPath= "Engine/Assets/HUD/Viewport/rotateIcon.png";
	const std::filesystem::path scaleIconPath = "Engine/Assets/HUD/Viewport/scaleIcon.png";
	const std::filesystem::path worldIconPath = "Engine/Assets/HUD/Viewport/worldIcon.png";
	const std::filesystem::path localIconPath = "Engine/Assets/HUD/Viewport/localIcon.png";
	const std::filesystem::path gridIconPath = "Engine/Assets/HUD/Viewport/gridIcon.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> translateSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rotateSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scaleSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> worldSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> localSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gridSRV;

	const std::filesystem::path playButtonPath = "Engine/Assets/HUD/Viewport/PlayButton.png";
	const std::filesystem::path stopButtonPath = "Engine/Assets/HUD/Viewport/StopButton.png";
	const std::filesystem::path pauseButtonPath = "Engine/Assets/HUD/Viewport/PauseButton.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> playBtnSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stopBtnSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pauseBtnSRV;


	ImVec4 activeButtonBG = { 0.258f, 0.588f, 1.0f, 1 };
	const char* elems_names[6] = { "0","0.1", "0.25", "0.5", "1.0","10.0" };
	const float snap_values[6] = { 0.0f,0.1f,0.25f,0.5f,1.0f,10.0f };
	int gridSnapParam = 0;
	float snap[3] = { 0,0,0 };

};

