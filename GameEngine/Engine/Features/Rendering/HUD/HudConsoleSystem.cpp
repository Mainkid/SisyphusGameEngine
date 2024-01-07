#include "HudConsoleSystem.h"
#include <filesystem>
#include "../../../Core/Tools/ErrorLogger.h"
#include "../../../Core/ServiceLocator.h"
#include "ImGui.h"

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


    return SyResult();
}

SyResult SyHudConsoleSystem::Run()
{
	OPTICK_EVENT();
	SyResult result;
	for (auto& message : SY_EL->messagePool)
		messageBuffer.Push(message);
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Console Output", NULL, ImGuiWindowFlags_NoTitleBar);
	ImGui::BeginChild("ScrollingLog", ImVec2(-ImGui::GetContentRegionAvail().x / 15, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	for (auto i = messageBuffer.Head(); messageBuffer.Stopped(i) != true; i = messageBuffer.Next(i))
	{
		auto& message = messageBuffer[i];
		ImVec4 color;
		bool has_color = false;
		char levelFirstChar = message.GetInsertion(1)[0];
		auto search = filters.find(levelFirstChar);
		if (search == filters.end())
		{
			SY_LOG_HUD(SY_LOGLEVEL_ERROR, "Failed to find %s in filters. ", &levelFirstChar);
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

	if (ImGui::BeginTable("options", 1))
	{
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
		//ImGui::TextUnformatted("");
		if (ImGui::Button("Clear", ImVec2(ImGui::GetContentRegionMax().x, 0)))
		{
			messageBuffer.Clear();
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
	ImGui::End();
	
	return result;
}

SyResult SyHudConsoleSystem::Destroy()
{
	return SyResult();
}
