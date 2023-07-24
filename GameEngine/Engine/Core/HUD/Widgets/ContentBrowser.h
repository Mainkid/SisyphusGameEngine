#pragma once
#include "../Widget.h"
#include "../../EngineCore.h"
#include <filesystem>



class ContentBrowser : public Widget
{

public:
	ContentBrowser(Hud* _hud);
	void Render() override;
	void GetInput() override {};

private:

	//Поменять это, когда будут проекты
	const std::string assetsDirectory = "Game/Assets";
	std::filesystem::path curDirectory = assetsDirectory;
};

