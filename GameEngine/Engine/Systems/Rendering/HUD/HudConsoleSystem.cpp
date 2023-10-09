#include "HudConsoleSystem.h"
#include <filesystem>
#include "../../../Tools/ErrorLogger.h"
#include "../../../Core/ServiceLocator.h"
SyResult SyHudConsoleSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	ec = ServiceLocator::instance()->Get<EngineContext>();

	filters = { {'I', true},
				{'D', true},
				{'W', true},
				{'E', true},
				{'C', true}};

	messages.reserve(10000);
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "HUD console system initialization successful. ");
	SY_LOG_HUD(SY_LOGLEVEL_WARNING, "Example warning! ");
	SY_LOG_HUD(SY_LOGLEVEL_ERROR, "Example error! ");
	SY_LOG_HUD(SY_LOGLEVEL_CRITICAL, "Example critical! ");
	SY_EL->AddChannel("FUCK");
	SY_LOG("FUCK", SY_LOGLEVEL_DEBUG, "Example fuck!");
    return SyResult();
}

SyResult SyHudConsoleSystem::Run()
{
	SyResult result;
	messages.insert(messages.end(), 
					SY_EL->messagePool.begin(), 
					SY_EL->messagePool.end());
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Console Output", NULL, ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::End();
		result.code = SY_RESCODE_ERROR;
		result.message = "Failed to create console output window. ";
		SY_LOG_HUD(SY_LOGLEVEL_ERROR, result.message.ToString());
		return result;
	};
	
	ImGui::BeginChild("ScrollingLog", ImVec2(-ImGui::GetContentRegionAvail().x / 15, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	for (auto& message : messages)
	{
		ImVec4 color;
		bool has_color = false;
		char levelFirstChar = message.GetInsertion(1)[0];
		auto search = filters.find(levelFirstChar);
		if (search == filters.end())
		{
			SY_LOG_HUD(SY_LOGLEVEL_ERROR, "Failed tp find %s in filters. ", &levelFirstChar);
			continue;
		}
		if (filters.find(levelFirstChar)->second == false)
			continue;
		if (levelFirstChar == 'E' || levelFirstChar == 'C')
			{ 
				color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); 
				has_color = true; 
			}
		else if (levelFirstChar == 'W')
		{
			color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
			has_color = true;
		}
		if (has_color)
			ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(message.ToString().c_str());
		if (has_color)
			ImGui::PopStyleColor();
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("Settings", ImVec2(0, 0), true);
	ImGui::TextUnformatted("Show:");
	ImGui::BeginTable("options", 1);
	ImGui::TableNextColumn();
	ImGui::Selectable("INFO", &(filters.find('I')->second));
	ImGui::TableNextColumn();
	ImGui::Selectable("DEBUG", &(filters.find('D')->second));
	ImGui::TableNextColumn();
	ImGui::Selectable("WARNING", &(filters.find('W')->second));
	ImGui::TableNextColumn();
	ImGui::Selectable("ERROR", &(filters.find('E')->second));
	ImGui::TableNextColumn();
	ImGui::Selectable("CRITICAL", &(filters.find('C')->second));
	ImGui::TableNextColumn();
	ImGui::Separator();
	ImGui::TextUnformatted("");
	ImGui::TextUnformatted("");
	if (ImGui::Button("Clear", ImVec2(ImGui::GetContentRegionMax().x, 0)))
	{
		messages.clear();
	}
	ImGui::EndTable();
	ImGui::EndChild();
	ImGui::End();
	return result;
}

SyResult SyHudConsoleSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "HUD console system destruction successful. ");
	return SyResult();
}