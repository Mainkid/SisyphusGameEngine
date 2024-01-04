#include <imgui.h>
#include <memory>
#include <vector>
#include "state_machine_graph.h"
#include "StateMachineLayer.h"
#include "StateMachineInteraction.h"
#include "StateMachinePainter.h"
#include "StateMachine.h"

using std::vector;

void ShowStateMachineGraph(bool* opened);
void initExampleNodes(StateMachineLayer &sMachine);

void ShowStateMachineGraph(bool* opened)
{
	ImGui::SetNextWindowSize(ImVec2(700, 700));

	/* begin a ImGui window */
	if (!ImGui::Begin("State Machine Graph", opened))
	{
		ImGui::End();
		return;
	}

	static StateMachine sMachine;
	
	static StateMachineLayer &baseLayer = sMachine.addLayer("Base Layer", true);
	initExampleNodes(baseLayer);

	static StateMachineInteraction canvas;
	canvas.updateFrame();

	static StateMachinePainter painter(&baseLayer, &canvas);

	ImGui::BeginGroup();
	{
		/*if (ImGui::Button("Save XML"))
		{
			baseLayer.XMLsave();
		}

		if (ImGui::Button("Load XML"))
		{
			baseLayer.XMLparse("save_file_output.xml");
		}*/
	}
	ImGui::EndGroup();

	ImGui::SameLine();

	//painter.drawLayerPanel(sMachine);

	ImGui::SameLine();

	painter.drawVariables();
	//painter.drawNodeListBar();

	ImGui::SameLine();

	painter.drawSideBar();

	ImGui::SameLine();

	painter.drawCanvas();

	//painter.drawNodeListBar();
	ImGui::End();
}

void initExampleNodes(StateMachineLayer &sMachine)
{
	if (sMachine.states.size() != 0)
		return;
	printf("check.\n");

	sMachine.addState("EntryPoint", ImVec2(140, 50), vector<StateID>{1});
	sMachine.addState("DefaultState", ImVec2(140, 150), vector<StateID>{});
}