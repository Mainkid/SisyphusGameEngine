#pragma once
#include <imgui.h>
#include <vector>
#include <set>
#include "Transition.h"
#include "pugixml.hpp"

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs);
inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);

typedef int StateID;

enum class StateType
{
	NORMAL,
	ENTRY,
	ANY,
	EXIT,
};

/* define node data structure. */
class State
{
public:
	/* id */
	StateID     id;

	/* appearance */
	ImVec2  pos, size;
	ImVec4  color;

	/* state */
	char    name[32];
	StateType type;
	std::set<TransitionID> transitions; /* transitions from this node to other nodes */
	bool isDefault;

	State()
	{

	}

	State(StateID id, const char* name, const ImVec2& pos, const std::set<TransitionID>& transitions, const StateType& type = StateType::NORMAL,
		const ImVec4& color = ImColor(0.3f, 0.3f, 0.8f))
	{
		this->id = id;
		strncpy_s(this->name, name, 31);
		this->name[31] = 0;
		this->pos = pos;
		this->transitions = transitions;

		this->type = type;
		this->color = color;

		this->isDefault = false;
	}


	State(StateID id, const char* name, const ImVec2& pos, const ImVec2& size, const ImVec4& color, StateType type, const std::set<TransitionID>& transitions, bool is_default)
		: id(id),
		pos(pos),
		size(size),
		color(color),
		type(type),
		transitions(transitions),
		isDefault(is_default)
	{
		strncpy_s(this->name, name, 31);
		this->name[31] = 0;
	}

	const char * typeStr() const {
		const char *_str[] = { "normal", "entry", "any", "exit" };

		return _str[int(type)];
	}

	ImVec2 drawPos() const {
		return center();
	}

	/* todo:you have to make sure the field.size is updated. */
	ImVec2 center() const {
		return pos + ImVec2(size.x / 2, size.y / 2);
	}

	ImVec2 anchorPos() const {
		return pos + ImVec2(size.x / 2, size.y + 5);
	}

	ImVec2 anchorScreenPos(ImVec2 canvasOrigin) const {
		return  canvasOrigin + pos + ImVec2(size.x / 2, size.y + 5);
	}

	ImVec2 rectMin(ImVec2 canvasOrigin) const
	{
		return canvasOrigin + pos;
	}

	ImVec2 rectMax(ImVec2 canvasOrigin) const
	{
		return canvasOrigin + pos + size;
	}

	void gen_xml_node(pugi::xml_node& root) const;

	//	static char* toString(float data, unsigned int precision = 4)
	//	{
	//		char format[50];
	//		sprintf(format, "%%.%df", precision);
	//		char res[50];
	//		sprintf(res, format, data);
	//
	//		return res;
	//	}

	//	static char* toXMLStr(ImVec2 vec)
	//	{
	//		char buf[100];
	//		sprintf(buf, "(%.6f, %.6f)", vec.x, vec.y);
	//		return buf;
	//	}
};