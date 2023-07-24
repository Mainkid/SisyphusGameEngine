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

	//�������� ���, ����� ����� �������
	const std::string assetsDirectory = "Game/Assets";
	std::filesystem::path curDirectory = assetsDirectory;
};

