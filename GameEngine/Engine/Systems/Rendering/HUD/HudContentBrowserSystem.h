#pragma once
#include "../../ISystem.h"
#include "imgui.h"
#include "ContentBrowserTypes.h"
#include <d3d11.h>
#include <map>
#include <set>
#include <filesystem>

struct EngineContext;
struct HardwareContext;
struct RenderContext;



class HudContentBrowserSystem : public ISystem
{
public:
    void Init() override;
    void Run() override;
    void Destroy() override;
private:
    EngineContext* ec;
    RenderContext* rc;
    HardwareContext* hc;
    void InitImagesSRV();
    void ProcessPopUp();

    ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);             // Black background
    ImVec4 bg_col_selected = ImVec4(0.52f, 0.73f, 1.0f, 1.0f);
    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint

    bool wasOverPopup = false;
    std::filesystem::path renaimingFileName = "";
    char renaimingFileString [64];

    const std::string folderIcon = "Engine/Assets/HUD/ContentBrowser/folder.png";
    const std::filesystem::path assetsDirectory = "Game/Assets";
    const float padding = 16.0;
    const float thumbnailSize = 128;
    const float cellSize = padding + thumbnailSize;
    std::filesystem::path curDirectory = assetsDirectory;
    ID3D11ShaderResourceView* folderImageSRV;
    std::map<EContent, ID3D11ShaderResourceView*> iconSRV;
    std::set<std::filesystem::path> selectedFiles;
    std::filesystem::path selectedFile;
};
