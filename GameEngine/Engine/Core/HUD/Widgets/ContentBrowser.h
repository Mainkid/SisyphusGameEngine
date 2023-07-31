#pragma once
#include "../Widget.h"
#include "ContentBrowserTypes.h"
#include "../../EngineCore.h"

#include <filesystem>


const std::string folderIcon = "Engine/Assets/HUD/ContentBrowser/folder.png";

class ContentBrowser : public Widget
{

public:
	ContentBrowser(Hud* _hud);
	void Render() override;
	void GetInput() override {};
	//TODO: Убрать в отдельный сервис-класс
	
	void InitImagesSRV();
	~ContentBrowser() override;

private:

	//TODO: Поменять это, когда будут проекты
	const std::filesystem::path assetsDirectory = "Game/Assets";
	const float padding = 16.0;
	const float thumbnailSize = 128;
	const float cellSize = padding + thumbnailSize;
	std::filesystem::path curDirectory = assetsDirectory;

	ID3D11ShaderResourceView* folderImageSRV;

	std::map<EContent, ID3D11ShaderResourceView*> iconSRV;
};

