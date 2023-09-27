#pragma once
#include <filesystem>
#include <fstream>
#include <string>

class ResourceHelper
{
public:
	static void ConstructFile(std::filesystem::path path)
	{
		int fileCtr = 0;
		if (!path.has_extension())
		{
			while (std::filesystem::exists(path.string() + std::to_string(fileCtr)))
				fileCtr++;
			std::filesystem::create_directory(path.string() + std::to_string(fileCtr));
			return;
		}

		std::ofstream file;
		
		std::filesystem::path pathCopy = path;
		std::string pathWithoutFile = path.remove_filename().string();
		path = pathCopy;
		std::string filenameWithoutExtension = path.filename().replace_extension().string();
		path = pathCopy;
		std::string extension = path.filename().extension().string();

		while (std::ifstream(pathWithoutFile+filenameWithoutExtension + std::to_string(fileCtr) + extension))
			++fileCtr;
		file.open(pathWithoutFile + filenameWithoutExtension + std::to_string(fileCtr) + extension, std::ios::out);
	}
	
	static void RemoveFile(std::filesystem::path path)
	{
		std::filesystem::remove_all(path);
	}

	static void MoveFile_(std::filesystem::path oldPath, std::filesystem::path newPath)
	{
		
		if (newPath.has_extension())
			return;

		auto filename = oldPath.filename();
		
		std::filesystem::rename(oldPath, newPath.string() +"/"+ filename.string());
	}
};

