#pragma once
#include <vector>
#include "State.h"

//inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
//inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

inline void State::gen_xml_node(pugi::xml_node& root) const
{
	auto _state_node = root.append_child("state");

	/* attributes */
	_state_node.append_attribute("id") = id;
	if (name)
		_state_node.append_attribute("name") = name;
	else
		_state_node.append_attribute("name") = "";
	_state_node.append_attribute("type") = unsigned int(type);

	/* positions */
	{
		auto _pos = _state_node.append_child("pos");
		_pos.append_attribute("x") = pos.x;
		_pos.append_attribute("y") = pos.y;
	}

	/* size */
	{
		auto _size = _state_node.append_child("size");
		_size.append_attribute("width") = size.x;
		_size.append_attribute("height") = size.y;
	}

	/* color */
	{
		auto _color = _state_node.append_child("color");
		_color.append_attribute("r") = color.x;
		_color.append_attribute("g") = color.y;
		_color.append_attribute("b") = color.z;
		_color.append_attribute("a") = color.w;
	}

	_state_node.append_attribute("isDefault") = isDefault;

	/* transitions */
	{
		auto _transitions = _state_node.append_child("transitions");
		for (TransitionID transition_id : transitions)
		{
			_transitions.append_attribute("id") = transition_id;
		}
	}
}

//inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
inline ImVec2 operator*(const ImVec2& lhs, const double& factor) { return ImVec2(lhs.x * factor, lhs.y * factor); }
inline ImVec2 operator*(const double& factor, const ImVec2& rhs) { return ImVec2(rhs.x * factor, rhs.y * factor); }
inline bool operator<(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x < rhs.x && lhs.y < rhs.y; }
#pragma warning (default: 4244)
void ShowStateMachineGraph(bool* opened);