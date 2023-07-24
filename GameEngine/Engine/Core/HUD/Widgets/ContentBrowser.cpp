#include "ContentBrowser.h"

ContentBrowser::ContentBrowser(Hud* _hud)
{
	this->hud = _hud;
}

void ContentBrowser::Render()
{
	ImGui::Begin("Content Browser");

	if (curDirectory != std::filesystem::path(assetsDirectory))
	{
		if (ImGui::Button("<-"))
		{
			curDirectory = curDirectory.parent_path();	
		}
	}

	for (auto& p : std::filesystem::directory_iterator(curDirectory))
	{
		std::string path = p.path().string();
		if (p.is_directory())
		{
			if (ImGui::Button(path.c_str()))
			{
				p.path().filename();
				curDirectory /= p.path().filename();
			}
		}
		else
		{
			if (ImGui::Button(path.c_str()))
			{

			}
		}
	}

	ImGui::End();
}
