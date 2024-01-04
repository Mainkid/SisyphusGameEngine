#include "StateMachinePainter.h"

void StateMachinePainter::drawLayerPanel(StateMachine& sMachine) {
	ImGui::BeginChild("layer_panel", ImVec2(200, 0));
	{
		ImGui::Text("Layers");
		ImGui::SameLine(ImGui::GetWindowWidth() - 20);
		if (ImGui::Button("+")) {
			sMachine.addLayer("New Layer");
		}

		for (StateMachineLayer &_layer : sMachine.layers) {
			ImGui::PushID(_layer.id); /* if you don't push/pop id, then imgui won't be able to distinguish InputText/Combo/... for different layers */

			auto draw_list = ImGui::GetWindowDrawList();
			draw_list->ChannelsSplit(2);

			ImGui::Separator();

			/* draw content of one layer in the panel */
			ImGui::BeginGroup();
			{
				draw_list->ChannelsSetCurrent(1);	/* draw content on foreground */
				ImGui::InputText("", _layer.name, sizeof(_layer.name) / sizeof(_layer.name[0]));
				const char *items[] = { "Override", "Additive" }; /* can be improved */
				ImGui::Text("Blending"); ImGui::SameLine();
				ImGui::Combo("", (int *)&_layer.blendMode, items, sizeof(items) / sizeof(items[0]));
			}
			ImGui::EndGroup();

			auto _rectMin = ImGui::GetItemRectMin();
			auto _rectMax = ImGui::GetItemRectMax();

			/* select layer */
			{
				bool _click_active = false;
				if (ImGui::IsMouseReleased(0) && _rectMin < ImGui::GetMousePos() && ImGui::GetMousePos() < _rectMax)
					_click_active = true;

				if (_click_active && interaction->getLayerSelected() != _layer.id) {
					printf("clicked, %s changed to %s\n", currentLayer->name, _layer.name);
					interaction->selectLayer(_layer.id);
					currentLayer = &_layer;
				}
			}

			if (interaction->getLayerSelected() == _layer.id) {
				draw_list->ChannelsSetCurrent(0);	/* draw highlight-background on background layer */
				draw_list->AddRectFilled(_rectMin, _rectMax, ImColor(55, 70, 74));	/* color and shape is still ugly */
			}
			draw_list->ChannelsMerge();

			ImGui::PopID();
		}
	}
	ImGui::EndChild();
}

bool StateMachinePainter::onTransitionLine(StateID from, StateID to, ImVec2 mousePos) const {
	auto from_pos = currentLayer->states[from].center();
	auto to_pos = currentLayer->states[to].center();

	auto A = to_pos.y - from_pos.y;
	auto B = to_pos.x - from_pos.x;
	auto C = to_pos.x * from_pos.y - from_pos.x * to_pos.y;
	double numerator = A * mousePos.x - B* mousePos.y + C;
	double denominator = A * A + B * B;
	double squre_dist = numerator / denominator;

	return squre_dist <= lineThickness * lineThickness ? true : false;
}

void StateMachinePainter::drawNodeListBar() const
{
	/* node list: left bar*/
	ImGui::BeginChild("node_list", ImVec2(100, 0));
	{
		ImGui::Text("Nodes");
		ImGui::Separator();

		for (auto& [stateId, state] : currentLayer->states)
		{
			State* node = &state;
			ImGui::PushID(node->id);
			/* draw and check if selected */
			if (ImGui::Selectable(node->name, node->id == interaction->getStateSelected()))
				interaction->selectState(node->id);
			/* check if Hovered. */
			if (ImGui::IsItemHovered()) {
				interaction->hoverStateList(node->id);
				interaction->setContextMenu();
			}
			ImGui::PopID();
		}
	}
	ImGui::EndChild();
}

void StateMachinePainter::drawSideBar() const
{
	/* draw param panel: side bar */
	ImGui::BeginChild("param panel", ImVec2(150, 0));
	{
		if (interaction->getStateSelected() >= 0) {
			ImGui::Text("State");
			State& _state = currentLayer->states[interaction->getStateSelected()];
			{
				ImGui::TextWrapped("Name: ");
				ImGui::SameLine();
				ImGui::InputText("##text", _state.name, sizeof(_state.name) / sizeof(_state.name[0]));

				ImGui::TextWrapped("Type: %s", _state.typeStr());
			}
			
			/* todo: should add multi transition support */
			if (ImGui::CollapsingHeader("Transitions", true))
			{
				for (TransitionID _transID : _state.transitions)
				{
					auto _trans = currentLayer->getTransition(_transID);
					ImGui::Text("%s -> %s", currentLayer->states[_trans.fromID].name, currentLayer->states[_trans.toID].name);
				}
				if (_state.transitions.size() == 0)
					ImGui::Text("Empty");

				
			}
		}

		if (interaction->getTransSelected() >= 0)
		{
			ImGui::Text("Transition");
			ImGui::Separator();
			Transition& _trans = currentLayer->getTransition(interaction->getTransSelected());
			ImGui::Text("%s -> %s", currentLayer->states[_trans.fromID].name, currentLayer->states[_trans.toID].name);
			ImGui::Checkbox("solo:", &_trans.solo);
			ImGui::Checkbox("mute:", &_trans.mute);
			ImGui::Checkbox("hasExit:", &_trans.hasExit);
			ImGui::Separator();
			ImGui::InputText("##TransitionName", _trans.animCondition->animParameterName.data(), _trans.animCondition->animParameterName.length());
		}
	}
	ImGui::EndChild();

}

void StateMachinePainter::drawCanvas()
{
	/* draw canvas */
	udpateCanvasOrigin();
	ImGui::BeginGroup();
	{
		ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
		ImGui::SameLine(ImGui::GetWindowWidth() - 100);
		ImGui::Checkbox("Show grid", &show_grid);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(60, 60, 70, 0.5f));

		ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
		{
			ImGui::PushItemWidth(120.0f);

			/* get current window draw list, and split into several layers */
			ImDrawList& draw_list = *ImGui::GetWindowDrawList();
			draw_list.ChannelsSplit(interaction->NUM_LAYERS);
			{
				/* draw grids */
				if (show_grid) { drawGrid(draw_list); }

				/* debug */
				if (interaction->node_debug)
				{
					draw_list.ChannelsSetCurrent(interaction->layer(0));	/* draw debug on top */
					ImGui::Text("offset pos %f, %f", canvas_origin.x, canvas_origin.y);
					ImGui::Text("mouse pos: %.2f %.2f", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

					for (std::pair<TransitionID, Transition> pair : currentLayer->transitions)
					{
						auto _trans = pair.second;
						auto from_pos = canvas_origin + currentLayer->states[_trans.fromID].center();
						auto to_pos = canvas_origin + currentLayer->states[_trans.toID].center();
						if (onTransitionLine(from_pos, to_pos, ImGui::GetMousePos()) && interaction->getStateHoveredInScene() < 0) {
							//						canvas->selectTrans(pair.first);
							ImGui::Text("hovered trans id %d", pair.first);
						}
						else
						{
							ImGui::Text("hovered nothing");
						}
					}

					if (interaction->getStateHoveredInScene() >= 0)
					{
						ImGui::Text("hoverd State.name: %s", currentLayer->states[interaction->getStateHoveredInScene()].name);
					}

					if (interaction->getStateSelected() >= 0) {
						State currState = currentLayer->states[interaction->getStateSelected()];
						ImGui::Text("State.name: %s", currState.name);
						for (TransitionID trans_id : currState.transitions)
						{
							auto _trans = currentLayer->getTransition(trans_id);
							ImGui::Text("\ttransition from %d to %d", _trans.fromID, _trans.toID);
						}
					}

					if (interaction->getTransSelected() >= 0)
					{
						Transition currTransition = currentLayer->getTransition(interaction->getTransSelected());
						ImGui::Text("Transition from %d to %d", currTransition.fromID, currTransition.toID);
					}

					if (ImGui::IsAnyItemHovered())
						ImGui::Text("Hovered");
				}

				/* draw states */
				for (auto& [_,node] : currentLayer->states)
					drawStateNode(draw_list, node);

				/* draw links */
				drawLinks(draw_list);

				/* check if right click on window context menu */
				interaction->checkWindowRightClick();

				/* select the corresponding node if context menu is opened on it */
				if (interaction->isContextMenuOpen())
				{
					ImGui::OpenPopup("context_menu");
					if (interaction->getStateHoveredInList() != -1) {
						interaction->selectState(interaction->getStateHoveredInList());
					}

					if (interaction->getStateHoveredInScene() != -1) {
						interaction->selectState(interaction->getStateHoveredInScene());
					}
				}

				/* Draw unfinished line */
				drawUnfinishedLine(draw_list);
			}
			draw_list.ChannelsMerge();

			/* Draw context menu */
			drawContextMenu();

			// Scrolling
			if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
				scrolling = scrolling - ImGui::GetIO().MouseDelta;

			ImGui::PopItemWidth();
		}
		ImGui::EndChild();

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

	}
	ImGui::EndGroup();
}

void StateMachinePainter::drawVariables()
{
	

	ImGui::BeginChild("node_list", ImVec2(100, 0));
	{
		ImGui::Text("Params");
		ImGui::SameLine(ImGui::GetWindowWidth() - 20);
		if (ImGui::Button("+")) {
			std::string name = "NewParam" + std::to_string(currentLayer->paramsVec.size());
			currentLayer->paramsVec.push_back(std::make_shared<AnimParam>(name));
		}
		ImGui::Separator();

		std::string sCtr = "##";

		for (auto& param : currentLayer->paramsVec)
		{
			const char* items[3] = { "bool","integer","float" };
			/* draw and check if selected */
			ImGui::InputText("##text", param->paramName.data(), param->paramName.length());
			//if (ImGui::Selectable(param->paramName.c_str(), false))
			//{
			//	std::cout << "";
			//}

			sCtr += param->paramName;
			
			int selected = int(param->paramType);
			//ImGui::SameLine(10);
			ImGui::Combo(sCtr.c_str(), &selected, items, sizeof(items) / sizeof(items[0]));

			param->paramType = EAnimParamType(selected);

			
		}

	}
	ImGui::EndChild();
}

void StateMachinePainter::drawStateNode(ImDrawList& draw_list, State& node) const {
	ImGui::PushID(node.id);

	draw_list.ChannelsSetCurrent(interaction->layer(0)); /* foreground */


	//		ImVec2 node_rect_min = canvas_origin + node.pos;
	
	ImVec2 node_rect_min = node.rectMin(canvas_origin);
	ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);

	/*draw node content*/
	bool old_any_active = ImGui::IsAnyItemActive();
	{
		
		
		ImGui::BeginGroup(); // Lock horizontal position
		
		ImGui::Text("%s", node.name);
		//ImGui::ColorEdit3("##color", &node.color.x);
		//				ImGui::Text("NodeSize: [%.2f, %.2f]", node.Size.x, node.Size.y);
		ImGui::EndGroup();
		
	}
	/* if the above widgets group (text, colorEdit, ...) is clicked, the whole node should also be active. */
	bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());

	/* update node size & pos */
	node.size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
	ImVec2 node_rect_max = node_rect_min + node.size;

	/* draw an invisible button to handle mouse input */
	{
		draw_list.ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node.size);		/* invisible button to check if mouse is hovered. */

		/* hovered, don't ask me why "|| node_widgets_active..." */
		if (ImGui::IsItemHovered() || node_widgets_active) {
			interaction->hoverStateScene(node.id);
			interaction->setContextMenu(); /* open right click on item context menu */
		}

		/* select node item */
		bool node_moving_active = ImGui::IsItemActive();
		bool right_click_select = ImGui::IsItemHovered() && ImGui::IsMouseClicked(1);	/* if you hover over the invisible button and right click, it's also select */

		if (node_widgets_active || node_moving_active || right_click_select)
			interaction->selectState(node.id);

		/* print node_selected to cmd */
		static int last_node_selected = -1;
		if (interaction->getStateSelected() != last_node_selected)
			printf("active id: %d\n", interaction->getStateSelected());
		last_node_selected = interaction->getStateSelected();


		if (node_moving_active && ImGui::IsMouseDragging(0))
			node.pos = node.pos + ImGui::GetIO().MouseDelta;
	}

	draw_list.ChannelsSetCurrent(interaction->layer(1));
	/* draw node background */
	{
		ImU32 node_bg_color = (interaction->getStateHoveredInList() == node.id || interaction->getStateHoveredInScene() == node.id
			|| (interaction->getStateHoveredInList() == -1 && interaction->getStateSelected() == node.id))
			? ImColor(75, 75, 75) : ImColor(60, 60, 60);

		draw_list.AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list.AddRect(node_rect_min, node_rect_max, ImColor(100, 100, 100), 4.0f);
	}

	ImGui::PopID();
}

void StateMachinePainter::drawGrid(ImDrawList& draw_list) const
{
	ImU32 GRID_COLOR = ImColor(200, 200, 200, 40);
	float GRID_SZ = 64.0f;
	ImVec2 win_pos = ImGui::GetCursorScreenPos();
	ImVec2 canvas_sz = ImGui::GetWindowSize();

	for (float x = fmodf(-scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
		draw_list.AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
	for (float y = fmodf(-scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
		draw_list.AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
}

void StateMachinePainter::drawLinks(ImDrawList& draw_list) const {
	draw_list.ChannelsSetCurrent(0); /* background */

	TransitionID transitionToDelete = -1;

	for (std::pair<TransitionID, Transition> pair : currentLayer->transitions)
	{
		auto _id = pair.first;
		auto _trans = pair.second;
		auto from_pos = canvas_origin + currentLayer->states[_trans.fromID].center();
		auto to_pos = canvas_origin + currentLayer->states[_trans.toID].center();
		auto mouse_pos = ImGui::GetMousePos();


		if (!interaction->hasDrawingLine && ImGui::IsMouseClicked(0) &&  interaction->getStateHoveredInScene() < 0) {
			if(_trans.fromID != _trans.toID && onTransitionLine(from_pos, to_pos, mouse_pos))
				interaction->selectTrans(_id);
			if (_trans.fromID == _trans.toID && onTransitionTriangle(currentLayer->states[_trans.fromID].anchorScreenPos(canvas_origin), mouse_pos))
				interaction->selectTrans(_id);
		}

		auto _color = ImColor(0, 0, 0);
		if (interaction->getTransSelected() == _id)
		{
			_color = ImColor(107, 178, 255);

			if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				if (_id != 0)
				{
					transitionToDelete = _id;
					interaction->selectTrans(-1);
				}
			}
		}
		else
			_color = ImColor(233, 233, 233);

		

		if (_trans.fromID == _trans.toID) {
			/* todo: transition from and to the same state */
			darwSingleTriangle(draw_list, currentLayer->states[_trans.fromID].anchorScreenPos(canvas_origin), _color);
		}
		else {
			draw_list.AddLine(from_pos, to_pos, _color, lineThickness);
			drawTriangleOnLine(draw_list, from_pos, to_pos, _color);
		}
	}

	if (transitionToDelete >= 0)
	{
		currentLayer->deleteTransition(transitionToDelete);
	}
}

void StateMachinePainter::drawUnfinishedLine(ImDrawList& draw_list) const {
	draw_list.ChannelsSetCurrent(interaction->layer(2)); /* mid layer */

	/* still drawing */
	if (interaction->hasDrawingLine)
	{
		interaction->drawing_line_end = ImGui::GetMousePos();
		draw_list.AddLine(interaction->drawing_line_start, interaction->drawing_line_end, interaction->darwing_line_color);
	}

	/* finish drawing */
	if (interaction->hasDrawingLine && ImGui::IsMouseClicked(0))
	{
		StateID transition_end_id = interaction->getStateHoveredInScene();
		interaction->hasDrawingLine = false;
		/* if you didn't click on any state */
		if (interaction->getStateHoveredInScene() != -1) {
			currentLayer->addTransitionAndUpdateState(interaction->transition_start_id, transition_end_id);
		}
	}
}

void StateMachinePainter::drawContextMenu() const {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		State* node = interaction->getStateSelected() != -1 ? &currentLayer->states[interaction->getStateSelected()] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - canvas_origin;
		if (node)
		{
			ImGui::Text("Node '%s'", node->name);
			ImGui::Separator();
			if (ImGui::MenuItem("New Transition", nullptr, false, true))
			{
				interaction->hasDrawingLine = true;
				interaction->transition_start_id = node->id;
				interaction->drawing_line_start = node->center() + canvas_origin;
				interaction->darwing_line_color = ImColor(100, 255, 255);
				printf("start form node id %d with node name %s\n", node->id, node->name);
			};
			if (ImGui::MenuItem("Delete", nullptr, false, interaction->getStateSelected()>1)) 
			{
				interaction->selectState(-1);
				currentLayer->deleteState(node->id);
			}
		}
		else
		{
			if (ImGui::MenuItem("Add")) { currentLayer->addState("New node", scene_pos, std::vector<StateID>{}); }
			if (ImGui::MenuItem("Paste", nullptr, false, false)) {}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

bool StateMachinePainter::onTransitionLine(ImVec2 from_pos, ImVec2 to_pos, ImVec2 mousePos) const {
	/* test if mouse go out of rect_range(from_pos, to_pos) */
	ImVec2 _test = ImVec2(ImVec2(mousePos - from_pos).x * ImVec2(mousePos - to_pos).x, ImVec2(mousePos - from_pos).y * ImVec2(mousePos - to_pos).y);
	if (_test.x > 0 || _test.y > 0)
		return false;

	double A = to_pos.y - from_pos.y;
	double B = -(to_pos.x - from_pos.x);
	double C = to_pos.x * from_pos.y - from_pos.x * to_pos.y;
	double numerator = A * mousePos.x + B* mousePos.y + C;
	double denominator = A * A + B * B;
	double squre_dist = fabs(numerator * numerator / denominator);

	/* use thickness/2 + 1 to better fit with visual effect */
	return (squre_dist <= (lineThickness / 2 + 1) * (lineThickness / 2 + 1)) ? true : false;
}

bool StateMachinePainter::onTransitionTriangle(ImVec2 anchor_pos, ImVec2 mousePos) {
	const double arrow_height = 10, arrow_width = 6;

	ImVec2 t1, t2, t3;
	t1 = anchor_pos;
	t2 = anchor_pos + ImVec2(arrow_width, arrow_height);
	t3 = anchor_pos + ImVec2(-arrow_width, arrow_height);

	return isPointInTriangle(mousePos, t1, t2, t3);
}

bool StateMachinePainter::isPointInTriangle(ImVec2 pt, ImVec2 t1, ImVec2 t2, ImVec2 t3) {
	double area = fabs(t1.x*(t2.y - t3.y) + t2.x*(t3.y - t1.y) + t3.x * (t1.y - t2.y)) / 2;

	double s = 1 / (2 * area)*(t1.y*t3.x - t1.x*t3.y + (t3.y - t1.y)*pt.x + (t1.x - t3.x)*pt.y);
	double t = 1 / (2 * area)*(t1.x*t2.y - t1.y*t2.x + (t1.y - t2.y)*pt.x + (t2.x - t1.x)*pt.y);

	return (0 <= s && s <= 1) && (0 <= t && t <= 1) && s + t <= 1 ? true : false;
}

/* the following two function shouldn't be here, move to painter later. */
void StateMachinePainter::drawTriangleOnLine(ImDrawList& draw_list, ImVec2 from_pos, ImVec2 to_pos, ImColor color, const double arrow_width)
{
	const double arrow_height = 10;

	ImVec2 t1, t2, t3;
	ImVec2 along, perp;

	auto norm = [](ImVec2 vec) -> ImVec2
	{
		double len = std::sqrt(vec.x * vec.x + vec.y * vec.y);
		return vec * (1 / len);
	};

	along = norm(to_pos - from_pos);
	perp = ImVec2(-along.y, along.x);

	t1 = (from_pos + to_pos) * 0.5;
	t2 = t1 - along * arrow_height + perp * arrow_width;
	t3 = t1 - along * arrow_height - perp * arrow_width;

	draw_list.AddTriangleFilled(t1, t2, t3, color);
}

void StateMachinePainter::darwSingleTriangle(ImDrawList& draw_list, const ImVec2 anchor_pos, const ImColor color) {
	const double arrow_height = 10, arrow_width = 6;

	ImVec2 t1, t2, t3;
	t1 = anchor_pos;
	t2 = anchor_pos + ImVec2(arrow_width, arrow_height);
	t3 = anchor_pos + ImVec2(-arrow_width, arrow_height);

	draw_list.AddTriangleFilled(t1, t2, t3, color);
}