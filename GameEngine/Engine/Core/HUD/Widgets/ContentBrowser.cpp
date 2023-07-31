#pragma once
#include "ContentBrowser.h"
#include "../../../Tools/ImageLoader.h"


ContentBrowser::ContentBrowser(Hud* _hud)
{
	this->hud = _hud;
	InitImagesSRV();
	
}

void ContentBrowser::Render()
{
	ImGui::Begin("Content Browser");
	Widget::Render();
	
	

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = max((int)panelWidth / cellSize, 1);
	ImGui::Columns(columnCount, 0, false);

	if (curDirectory != std::filesystem::path(assetsDirectory))
	{
		ImGui::ImageButton("...", iconSRV[EContent::Folder],{ thumbnailSize,thumbnailSize });
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			curDirectory = curDirectory.parent_path();
		}
		ImGui::Text("...");
		ImGui::NextColumn();
	}

	

	for (auto& directoryEntry : std::filesystem::directory_iterator(curDirectory))
	{
		const auto& path = directoryEntry.path();
		auto relativePath=std::filesystem::relative(directoryEntry.path(), assetsDirectory);
		std::string  fileNameStr = relativePath.filename().string();
		std::string fileExtension =relativePath.extension().string();

		ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
		ImVec2 uv1 = ImVec2(128.0f / 128.0f, 128.0f / 128.0f);    // UV coordinates for (32,32) in our texture
		ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);             // Black background
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint
		ImGui::ImageButton("a",iconSRV[cb_extensionsDB[fileExtension]], ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 0), ImVec2(1, 1), bg_col, tint_col);

		
		if (ImGui::IsItemHovered()	 && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (directoryEntry.is_directory())
				curDirectory /= path.filename();
		}
		ImGui::Text(fileNameStr.c_str());



		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::End();
}

void ContentBrowser::InitImagesSRV()
{
	int size;
	for (auto& keyVal : cb_iconsPathsDB)
	{
		iconSRV[keyVal.first] = nullptr;
		ImageLoader::LoadTextureFromFile(keyVal.second.string().c_str(), &iconSRV[keyVal.first], &size, &size);
	}
	std::cout << std::endl;
}

ContentBrowser::~ContentBrowser()
{

}
