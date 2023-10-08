#pragma once
#include "../../ISystem.h"
#include "imgui.h"
#include "imgui_internal.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "ContentBrowserTypes.h"
#include "ResourceHelper.h"
#include <d3d11.h>
#include <map>
#include <unordered_map>
#include <set>
#include <filesystem>

struct EngineContext;
struct HardwareContext;
struct RenderContext;
class ResourceService;



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
    ResourceService* rs;
    void InitImagesSRV();
    void FillPathToAssetMap(std::filesystem::path path);
    void ProcessPopUp();
    void DrawTreeFolderWindow(float windowLeftSizeX);
    void RenderTree(std::filesystem::path path);
    bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);
    void InitializePathFileViews(const std::filesystem::path& path);
    void PrintDirectory();

    ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);             // Black background
    ImVec4 bg_col_selected = ImVec4(0.52f, 0.73f, 1.0f, 1.0f);
    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint

    ImVec2 selectionStart;
    ImVec2 selectionEnd;

    bool wasOverPopup = false;
    bool itemWasHovered = false;
    bool areaSelected = false;
    
    std::filesystem::path renamingFileName = "";
    char renamingFileString [64];

    const std::string folderIcon = "Engine/Assets/HUD/ContentBrowser/folder.png";
    const std::filesystem::path assetsDirectory = ".\\Game\\Assets";
    const float padding = 16.0;
    const float thumbnailSize = 128;
    const float cellSize = padding + thumbnailSize;
    const float startLeftWindowWidth = 150;
    std::filesystem::path curDirectory = assetsDirectory;
    ID3D11ShaderResourceView* folderImageSRV;
    std::map<EAssetType, ID3D11ShaderResourceView*> iconSRV;
    std::unordered_map<std::string, EAssetType> pathToAssetTypeMap;
    std::set<std::filesystem::path> selectedFiles;
    std::vector<std::filesystem::directory_entry> fileViewsVec;
    std::filesystem::path selectedFile;
};
