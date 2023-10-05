#include "HudConsoleSystem.h"

#include "../../../Tools/ErrorLogger.h"
#include "../../../Core/ServiceLocator.h"
SyResult SyHudConsoleSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	ec = ServiceLocator::instance()->Get<EngineContext>();

	messages = { "a", "ERROR", "WARNING" };
    return SyResult();
}

SyResult SyHudConsoleSystem::Run()
{
	SyResult result;
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
	return result;
}

SyResult SyHudConsoleSystem::Destroy()
{
	return SyResult();
}
