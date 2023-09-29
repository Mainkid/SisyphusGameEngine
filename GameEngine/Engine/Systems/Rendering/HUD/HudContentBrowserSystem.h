#pragma once
#include "../../ISystem.h"
#include "ContentBrowserTypes.h"
#include <d3d11.h>
#include <map>
#include <filesystem>

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class HudContentBrowserSystem : public ISystem
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;
private:
    EngineContext* ec;
    RenderContext* rc;
    HardwareContext* hc;
    void InitImagesSRV();

    const std::string folderIcon = "Engine/Assets/HUD/ContentBrowser/folder.png";
    const std::filesystem::path assetsDirectory = "Game/Assets";
    const float padding = 16.0;
    const float thumbnailSize = 128;
    const float cellSize = padding + thumbnailSize;
    std::filesystem::path curDirectory = assetsDirectory;
    ID3D11ShaderResourceView* folderImageSRV;
    std::map<EContent, ID3D11ShaderResourceView*> iconSRV;
};
