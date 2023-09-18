#pragma once
#include <map>
#include <filesystem>

enum class EContent
{
	None,
	Folder,
	Mesh,
	Image
};



static std::map<EContent, std::filesystem::path> cb_iconsPathsDB{
	{EContent::None,"Engine/Assets/HUD/ContentBrowser/meshIcon.png"},
	{EContent::Folder, "Engine/Assets/HUD/ContentBrowser/folderIcon.png"},
	{EContent::Image, "Engine/Assets/HUD/ContentBrowser/imageIcon.png"},
	{EContent::Mesh, "Engine/Assets/HUD/ContentBrowser/meshIcon.png"}
};

static std::map<std::string, EContent> cb_extensionsDB{

	{"",EContent::Folder},
	{".obj",EContent::Mesh},
	{".fbx",EContent::Mesh},
	{".png",EContent::Image},
	{".jpeg",EContent::Image},
};