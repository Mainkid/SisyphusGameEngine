#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "../../Core/ECS/SystemBase.h"
#include "ContentBrowserTypes.h"
#include "ResourceHelper.h"
#include <d3d11.h>
#include <map>
#include <unordered_map>
#include <set>
#include <filesystem>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>

struct EngineContext;
struct HardwareContext;
struct RenderContext;
class ResourceService;

struct FileViews
{
    std::filesystem::directory_entry fileView;
    std::filesystem::path relativePath;
};

struct DirectoryTree
{
    std::filesystem::path path;
    std::vector<DirectoryTree> subDirs;
};

class HudContentBrowserSystem : public SystemBase
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
    void InitImagesSRV();
    void ProcessPopUp();
    void DrawTreeFolderWindow(float windowLeftSizeX);
    void RenderTree(DirectoryTree& dirTr);
    void InitializeDirectoryTree(std::filesystem::path path, DirectoryTree& dirTree);
    bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);
    void InitializePathFileViews(const std::filesystem::path& path);
    void UpdatePathFileViewsEvent(bool);
    void UpdateFolderHierarchyEvent(bool);
    bool CheckRemovingResourceReferences(const std::filesystem::path& path);

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
    //std::unordered_map<std::string, EAssetType> pathToAssetTypeMap;
    std::set<std::filesystem::path> selectedFiles;
    std::filesystem::path selectedFile;

    std::vector<FileViews> fileViewsVec;
    std::vector<EAssetType> fileAssetTypeVec;
    DirectoryTree dirTree;
};
