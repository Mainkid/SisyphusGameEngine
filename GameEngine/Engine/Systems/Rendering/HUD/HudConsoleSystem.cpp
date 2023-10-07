#include "HudConsoleSystem.h"
#include <filesystem>
#include "../../../Tools/ErrorLogger.h"
#include "../../../Core/ServiceLocator.h"
SyResult SyHudConsoleSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	ec = ServiceLocator::instance()->Get<EngineContext>();

	messages = { "a", "ERROR", "WARNING" };
	std::filesystem::path logDir_ = SY_EL->logFileDir;
	for (auto& file : std::filesystem::directory_iterator(logDir_))
		logDir = file.path();
	fin = std::ifstream(logDir);
	if (!fin.is_open())
		SY_LOG_CORE(SY_LOGLEVEL_ERROR, L"Failed to open log file. ");
	SY_LOG_CORE(SY_LOGLEVEL_INFO, L"HUD console system initialization successful. ");
    return SyResult();
}

SyResult SyHudConsoleSystem::Run()
{
	SyResult result;
	while (!fin.eof())
	{
		std::string message;
		fin >> message;
		messages.push_back(message);
	}
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Console Output"))
	{
		ImGui::End();
		result.code = SY_RESCODE_ERROR;
		result.message = L"Failed to create console output window. ";
		SY_LOG_HUD(SY_LOGLEVEL_ERROR, result.message.c_str());
		return result;
	};
	ImGui::BeginChild("ScrollingLog", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	for (auto& message : messages)
	{
		ImVec4 color;
		bool has_color = false;
		if (message.find("ERROR") != std::string::npos)
			{ 
				color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); 
				has_color = true; 
			}
		else if (message.find("WARNING") != std::string::npos)
		{
			color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
			has_color = true;
		}
		if (has_color)
			ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(message.c_str());
		if (has_color)
			ImGui::PopStyleColor();
	}
	ImGui::EndChild();
	ImGui::End();
	fin.seekg(0);
	messages.clear();
	return result;
}

SyResult SyHudConsoleSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, L"HUD console system destruction successful. ");
	return SyResult();
}
